#include "tests/t-index.h"

#include <iostream>

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

void anotherFunction() {
    constexpr addr_size BUF_MAX = core::KILOBYTE * 4;
    char buf[BUF_MAX] = {};
    addr_size bufWritten = 0;

    bool ok = core::stacktrace(buf, BUF_MAX, bufWritten, 20);
    std::cout << buf << std::endl;
}

void someFunction() {
    anotherFunction();
}

i32 main() {
    someFunction();
    return 0;
}
