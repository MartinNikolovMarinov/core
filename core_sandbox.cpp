#include "core_test_init.h"

#include <iostream>

namespace core {
    template <typename T> u32 hash(const T& val) = delete;
}

template <typename T>
concept Squareable =
    requires {
        requires core::is_same_v<decltype(&T::square), i32 (T::*)(i32)> ||
                 core::is_same_v<decltype(&T::square), f64 (T::*)(f64)>;
    };

template <typename T>
concept Hashable32bit = requires(const T& a) {
    { core::hash<T>(a) } -> core::same_as<u32>;
};

template <typename T>
concept SquareHashable = Squareable<T> && Hashable32bit<T>;

struct A {
    i32 square(i32 x) {
        return x * x;
    }
};

namespace core {
    template <>
    u32 hash<A>(const A&) {
        return 0;
    }
};

struct B {
    i64 square(i32 x) {
        return x * x;
    }
};

struct C {};

struct D {
    f64 square(f64 x) {
        return x * x;
    }
};

namespace core {
    u64 hash(const D&) {
        return 0;
    }
};

i32 main() {
    std::cout << std::boolalpha;

    std::cout << "A is Squareable " << Squareable<A> << "\n";
    std::cout << "B is Squareable " << Squareable<B> << "\n";
    std::cout << "C is Squareable " << Squareable<C> << "\n";
    std::cout << "D is Squareable " << Squareable<D> << "\n";

    std::cout << "A is 32bit Hashable = " << Hashable32bit<A> << "\n";
    std::cout << "B is 32bit Hashable = " << Hashable32bit<B> << "\n";
    std::cout << "C is 32bit Hashable = " << Hashable32bit<C> << "\n";
    std::cout << "D is 32bit Hashable = " << Hashable32bit<D> << "\n";

    std::cout << "A is Squareable and 32bit Hashable = " << SquareHashable<A> << "\n";
    std::cout << "B is Squareable and 32bit Hashable = " << SquareHashable<B> << "\n";
    std::cout << "C is Squareable and 32bit Hashable = " << SquareHashable<C> << "\n";
    std::cout << "D is Squareable and 32bit Hashable = " << SquareHashable<D> << "\n";

    return 0;
}
