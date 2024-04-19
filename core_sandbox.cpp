#include "tests/t-index.h"

#include <iostream>
#include <thread>

// namespace core {
//     template <typename T> u32 hash(const T& val) = delete;
// }

// template <typename T>
// concept Squareable =
//     requires {
//         requires core::is_same_v<decltype(&T::square), i32 (T::*)(i32)> ||
//                  core::is_same_v<decltype(&T::square), f64 (T::*)(f64)>;
//     };

// template <typename T>
// concept Hashable32bit = requires(const T& a) {
//     { core::hash<T>(a) } -> core::same_as<u32>;
// };

// template <typename T>
// concept SquareHashable = Squareable<T> && Hashable32bit<T>;

// struct A {
//     i32 square(i32 x) {
//         return x * x;
//     }
// };

// namespace core {
//     template <>
//     u32 hash<A>(const A&) {
//         return 0;
//     }
// };

// struct B {
//     i64 square(i32 x) {
//         return x * x;
//     }
// };

// struct C {};

// struct D {
//     f64 square(f64 x) {
//         return x * x;
//     }
// };

// namespace core {
//     u64 hash(const D&) {
//         return 0;
//     }
// };

thread_local u8 tl_buffer [1024];
core::ThreadLocalBumpAllocator g_bumpAllocator = core::ThreadLocalBumpAllocator::create();

i32 main() {
    defer { g_bumpAllocator.clear(); };

    g_bumpAllocator.setBuffer(tl_buffer, sizeof(tl_buffer));
    u8* one = reinterpret_cast<u8*>(g_bumpAllocator.calloc(1024, sizeof(u8)));
    if (one) {
        std::cout << "Main: Allocated 1024 bytes at " << one << std::endl;
    }

    std::thread t1([]() {
        defer { g_bumpAllocator.clear(); };

        g_bumpAllocator.setBuffer(tl_buffer, sizeof(tl_buffer));
        u8* one2 = reinterpret_cast<u8*>(g_bumpAllocator.alloc(1024, sizeof(u8)));
        if (one2) {
            std::cout << "T1: Allocated 1024 bytes at " << one2 << std::endl;
        }

        core::memset(one2, 1, 1024);
        auto first = one2[0];
        auto last = one2[1023];
        Panic(first == 1);
        Panic(last == 1);
    });

    std::thread t2([]() {
        defer { g_bumpAllocator.clear(); };

        g_bumpAllocator.setBuffer(tl_buffer, sizeof(tl_buffer));
        u8* one2 = reinterpret_cast<u8*>(g_bumpAllocator.alloc(1024, sizeof(u8)));
        if (one2) {
            std::cout << "T2: Allocated 1024 bytes at " << one2 << std::endl;
        }

        core::memset(one2, 2, 1024);
        auto first = one2[0];
        auto last = one2[1023];
        Panic(first == 2);
        Panic(last == 2);
    });

    auto first = one[0];
    auto last = one[1023];
    Panic(first == 0);
    Panic(last == 0);

    t1.join();
    t2.join();

    return 0;
}
