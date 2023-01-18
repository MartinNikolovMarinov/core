#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

template <typename...> struct tuple;

template <typename T1>
struct CORE_API_EXPORT tuple<T1> {
    static constexpr u32 len = 1;

    constexpr tuple() : v1({}) {}
    constexpr tuple(T1&& a) : v1(a) {}

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
    T1 v1;
};

template <typename T1, typename T2>
struct CORE_API_EXPORT tuple<T1, T2> {
    static constexpr u32 len = 2;

    constexpr tuple() : v1({}), v2({}) {}
    constexpr tuple(T1&& a, T2&& b) : v1(a), v2(b) {}

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
    T1 v1;
    T2 v2;
};

template <typename T1, typename T2, typename T3>
struct CORE_API_EXPORT tuple<T1, T2, T3> {
    static constexpr u32 len = 3;

    constexpr tuple() : v1({}), v2({}), v3({}) {}
    constexpr tuple(T1&& a, T2&& b, T3&& c) : v1(a), v2(b), v3(c) {}

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
    T1 v1;
    T2 v2;
    T3 v3;
};

template <typename T1, typename T2, typename T3, typename T4>
struct CORE_API_EXPORT tuple<T1, T2, T3, T4> {
    static constexpr u32 len = 4;

    constexpr tuple() : v1({}), v2({}), v3({}), v4({}) {}
    constexpr tuple(T1&& a, T2&& b, T3&& c, T4&& d) : v1(a), v2(b), v3(c), v4(d) {}

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
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
};

template <typename...TArgs>
constexpr CORE_API_EXPORT tuple<TArgs...> create_tuple(TArgs&&... args) {
    return tuple<TArgs...>(core::forward<TArgs>(args)...);
}

} // namespace core
