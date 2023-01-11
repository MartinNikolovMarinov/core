#include "API.h"

#include "useful_casting.h"
#include "move_and_forward.h"

#include <types.h>

namespace core {

using namespace coretypes;

template <typename...> CORE_API_EXPORT struct tuple;

template <typename T1>
struct CORE_API_EXPORT tuple<T1> {
    template <i32 TIdx>
    constexpr auto& get() {
        if constexpr (TIdx == 0) return v1;
    }
    template <i32 TIdx>
    constexpr const auto& get() const {
        // NOTE:
        // Can't reuse the non-const version because it uses auto + constexpr and can't deduce the type.
        // Or it takes too much efford to research how to do this and I am lazy.
        // Either way, copy paste like an idiot. Thanks C++.
        if constexpr (TIdx == 0) return v1;
    }

    // friends
    template <typename...T> friend constexpr tuple<T...> create_tuple(T&&... args);

private:
    constexpr tuple() : v1({}) {}
    constexpr tuple(T1&& a) : v1(a) {}

    T1 v1;
};

template <typename T1, typename T2>
struct CORE_API_EXPORT tuple<T1, T2> {
    template <i32 TIdx>
    constexpr auto& get() {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
    }
    template <i32 TIdx>
    constexpr const auto& get() const {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
    }

    // friends
    template <typename...T> friend constexpr tuple<T...> create_tuple(T&&... args);

private:
    constexpr tuple() : v1({}), v2({}) {}
    constexpr tuple(T1&& a, T2&& b) : v1(a), v2(b) {}

    T1 v1;
    T2 v2;
};

template <typename T1, typename T2, typename T3>
struct CORE_API_EXPORT tuple<T1, T2, T3> {
    template <i32 TIdx>
    constexpr auto& get() {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
        else if constexpr (TIdx == 2) return v3;
    }
    template <i32 TIdx>
    constexpr const auto& get() const {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
        else if constexpr (TIdx == 2) return v3;
    }

    // friends
    template <typename...T> friend constexpr tuple<T...> create_tuple(T&&... args);

private:
    constexpr tuple() : v1({}), v2({}), v3({}) {}
    constexpr tuple(T1&& a, T2&& b, T3&& c) : v1(a), v2(b), v3(c) {}

    T1 v1;
    T2 v2;
    T3 v3;
};

template <typename T1, typename T2, typename T3, typename T4>
struct CORE_API_EXPORT tuple<T1, T2, T3, T4> {
    template <i32 TIdx>
    constexpr auto& get() {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
        else if constexpr (TIdx == 2) return v3;
        else if constexpr (TIdx == 3) return v4;
    }
    template <i32 TIdx>
    constexpr const auto& get() const {
        if      constexpr (TIdx == 0) return v1;
        else if constexpr (TIdx == 1) return v2;
        else if constexpr (TIdx == 2) return v3;
        else if constexpr (TIdx == 3) return v4;
    }

    // friends
    template <typename...T> friend constexpr tuple<T...> create_tuple(T&&... args);

private:
    constexpr tuple() : v1({}), v2({}), v3({}), v4({}) {}
    constexpr tuple(T1&& a, T2&& b, T3&& c, T4&& d) : v1(a), v2(b), v3(c), v4(d) {}

    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
};

template <typename...TArgs>
constexpr CORE_API_EXPORT tuple<TArgs...> create_tuple(TArgs&&... args) {
    return tuple<TArgs...>(core::forward<TArgs>(args)...);
}

}

// EXAMPLE USAGE -------------------------------------------------------------------------------------------------------

#include <iostream>
#include <core.h>

using namespace coretypes;

struct A {
    A(i32 a, f32 b, std::string c) : a(a), b(b), c(c) {
        std::cout << "Obisously can't happen during compilation time!" << std::endl;
    }

    i32 a;
    f32 b;
    std::string c;

    bool operator==(const A& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
    bool operator!=(const A& other) const {
        return !(*this == other);
    }
};

void tuple_usage() {
    constexpr auto t1 = core::create_tuple(f64(6.0));
    auto t2 = core::create_tuple(1, A{2, 3.0f, "String that is larger than the small string optimization"});
    auto t3 = core::create_tuple(1, A{2, 3.0f, "Second string that is larger than the small string optimization"}, (u64)(6));
    auto t4 = t3;

    auto& ref = t3.get<2>();
    ref++;
    std::cout << t3.get<2>() << std::endl;

    constexpr auto test = t1.get<0>();
    std::cout << test << std::endl;

    t3.get<1>().c = "Changed string";

    std::cout << (t2.get<0>() + t3.get<0>()) << std::endl;
    std::cout << (t2.get<1>().a + t3.get<1>().b) << std::endl;
    std::cout << t3.get<1>().c << std::endl;
    std::cout << (t4.get<0>() == t3.get<0>() && t4.get<1>() != t3.get<1>() && t4.get<2>() != t3.get<2>() ? "true" : "false") << std::endl;
}
