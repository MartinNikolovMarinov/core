#pragma once

#include <types.h>

#include <cmath>
#include <type_traits>

namespace core::math
{

// FIXME: write tests for this!

using namespace coretypes;

template<i32 Dim, typename T> struct vec;

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vadd(vec<Dim, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] += static_cast<TDst>(src);
        }
    } else {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] += static_cast<TDst>(src[i]);
        }
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vsub(vec<Dim, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] -= static_cast<TDst>(src);
        }
    } else {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] -= static_cast<TDst>(src[i]);
        }
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vmul(vec<Dim, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] *= static_cast<TDst>(src);
        }
    } else {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] *= static_cast<TDst>(src[i]);
        }
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vdiv(vec<Dim, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] /= static_cast<TDst>(src);
        }
    } else {
        for (i32 i = 0; i < dst.size(); ++i) {
            dst[i] /= static_cast<TDst>(src[i]);
        }
    }
}

template<typename T, i32 Dim>
constexpr u64 vlength(const vec<Dim, T>& v) {
    u64 ret = 0;
    for (i32 i = 0; i < v.size(); ++i) {
        ret += v[i] * v[i];
    }
    ret = std::sqrt(ret);
    return ret;
}

template<typename T, i32 Dim>
constexpr void vnegate(vec<Dim, T>& v) {
    for (i32 i = 0; i < v.size(); ++i) {
        v[i] = -v[i];
    }
}

template<typename T1, typename T2, i32 Dim>
constexpr u64 vdot(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    u64 ret = 0;
    for (i32 i = 0; i < v1.size(); ++i) {
        ret += v1[i] * static_cast<T1>(v2[i]);
    }
    return ret;
}

template<typename T1, typename T2, i32 Dim>
constexpr vec<Dim, T1> vcross(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    vec<Dim, T1> ret;
    for (i32 i = 0; i < v1.size(); ++i) {
        ret[i] = v1[(i + 1) % v1.size()] * v2[(i + 2) % v1.size()] -
                 v1[(i + 2) % v1.size()] * v2[(i + 1) % v1.size()];
    }
    return ret;
}

template<typename T1, typename T2, i32 Dim>
constexpr bool vequals(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    for (i32 i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] != static_cast<T1>(v2[i]))) return false;
    }
    return true;
}

template<typename T1, typename T2, i32 Dim>
constexpr bool vsafeequals(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2, T1 epsilon = 0) {
    for (i32 i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] - static_cast<T1>(v2[i])) > epsilon) return false;
    }
    return true;
}

template<i32 Dim, typename T>
struct vec {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
    static_assert(Dim > 0, "Dim must be greater than 0");

    T data[Dim] = {};

    constexpr vec() = default;
    template<typename ...Args>
    constexpr vec(Args... args) {
        static_assert(sizeof...(Args) == Dim, "Invalid number of arguments");
        i32 i = 0;
        auto f = [this, &i](auto arg) {
            data[i++] = static_cast<T>(arg);
        };
        (f(args), ...);
    }

    template<i32 D = Dim, typename std::enable_if<(D > 0), int>::type = 0> T x() { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), int>::type = 0> T y() { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), int>::type = 0> T z() { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), int>::type = 0> T w() { return data[3]; }

    constexpr i32 size() const { return Dim; }

    constexpr T& operator[](i32 i) {
        Assert(i >= 0 && i < Dim, "Index out of bounds");
        return data[i];
    }
    constexpr const T& operator[](i32 i) const {
        Assert(i >= 0 && i < Dim, "Index out of bounds");
        return data[i];
    }

    constexpr bool equals(const vec<Dim, T>& other) const            { return vequals(*this, other); }
    constexpr bool equals(const vec<Dim, T>& other, T epsilon) const { return vsafeequals(*this, other, epsilon); }

    constexpr void negate() { vnegate(*this); }

    template<typename U>  constexpr void add(const vec<Dim, T>& other) { vadd(*this, other); }
    template<typename TA> constexpr void add(TA v)                     { vadd(*this, v); }

    template<typename U>  constexpr void sub(const vec<Dim, T>& other) { vsub(*this, other); }
    template<typename TA> constexpr void sub(TA v)                     { vsub(*this, v); }

    template<typename U>  constexpr void mul(const vec<Dim, T>& other) { vmul(*this, other); }
    template<typename TA> constexpr void mul(TA v)                     { vmul(*this, v); }
    template<typename TA> constexpr void scale(TA v)                   { mul(v); }

    template<typename U>  constexpr void div(const vec<Dim, U>& other) { vmul(*this, other); }
    template<typename TA> constexpr void div(TA v)                     { vmul(*this, v); }

    [[nodiscard]] constexpr u64 length() const { return vlength(*this); }

    template<typename U>
    [[nodiscard]] constexpr u64 dot(const vec<Dim, U>& other) const { return vdot(*this, other); }

    template<typename U>
    [[nodiscard]] constexpr vec<Dim, T> cross(const vec<Dim, U>& other) const { return vcross(*this, other); }
};

template<typename T> using vec2 = vec<2, T>;
template<typename T> using vec3 = vec<3, T>;

} // namespace core::math
