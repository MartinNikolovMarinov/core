#pragma once

#include <API.h>
#include <types.h>

namespace core
{

using namespace coretypes;

template <typename...> struct CORE_API_EXPORT Tuple;

template <typename T1, typename T2>
struct CORE_API_EXPORT Tuple<T1, T2> {

    constexpr Tuple(T1&& a, T2&& b) : a(a), b(b) {}

    // no copy
    constexpr Tuple(const Tuple&) = delete;
    constexpr Tuple& operator=(const Tuple&) = delete;

    T1& First()  { return a; }
    T2& Second() { return b; }

private:
    T1 a;  T2 b;
};

template <typename T1, typename T2, typename T3>
struct CORE_API_EXPORT Tuple<T1, T2, T3> {

    constexpr Tuple(T1&& a, T2&& b, T3&& c) : a(a), b(b), c(c) {}

    // no copy
    constexpr Tuple(const Tuple&) = delete;
    constexpr Tuple& operator=(const Tuple&) = delete;

    T1& First()  { return a; }
    T2& Second() { return b; }
    T3& Third()  { return c; }

private:
    T1 a; T2 b; T3 c;
};

template <typename T1, typename T2, typename T3, typename T4>
struct CORE_API_EXPORT Tuple<T1, T2, T3, T4>  {

    constexpr Tuple(T1&& a, T2&& b, T3&& c, T4&& d) : a(a), b(b), c(c), d(d) {}

    // no copy
    constexpr Tuple(const Tuple&) = delete;
    constexpr Tuple& operator=(const Tuple&) = delete;

    T1& First()  { return a; }
    T2& Second() { return b; }
    T3& Third()  { return c; }
    T4& Fourth() { return d; }

private:
    T1 a; T2 b; T3 c; T4 d;
};

} // namespace core
