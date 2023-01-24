#pragma once

#include <types.h>
#include <std/traits.h>

#include <cmath>

namespace core {

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
            Assert(static_cast<TDst>(src) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
            dst[i] /= static_cast<TDst>(src);
        }
    } else {
        for (i32 i = 0; i < dst.size(); ++i) {
            Assert(static_cast<TDst>(src[i]) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
            dst[i] /= static_cast<TDst>(src[i]);
        }
    }
}

template<typename T, i32 Dim>
constexpr f64 vlength(const vec<Dim, T>& v) {
    f64 ret = 0;
    for (i32 i = 0; i < v.size(); ++i) {
        ret += static_cast<f64>(v[i]) * static_cast<f64>(v[i]);
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
constexpr f64 vdot(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    f64 ret = 0;
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
        if (v1[i] != static_cast<T1>(v2[i])) return false;
    }
    return true;
}

template<typename T1, typename T2, i32 Dim>
constexpr bool vsafeequals(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2, T1 epsilon) {
    for (i32 i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] - static_cast<T1>(v2[i])) > epsilon) return false;
    }
    return true;
}

template<typename T, i32 Dim>
constexpr vec<Dim, T> vnorm(const vec<Dim, T>& v) {
    f64 len = vlength(v);
    if (len == 0) return v.zero();
    vec<Dim, T> ret = v;
    for (i32 i = 0; i < v.size(); ++i) {
        ret[i] /= static_cast<T>(len);
    }
    return ret;
}

template<i32 Dim, typename T>
struct vec {
    static_assert(std::is_trivial_v<T>, "T must be trivial");
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
    static_assert(Dim > 0, "Dim must be greater than 0");

    T data[Dim] = {};

    constexpr vec() = default;

    template<typename ...Args>
    constexpr vec(Args... args) {
        using commonT = template_args_common_type<Args...>;
        static_assert(std::is_same_v<commonT, T>, "All arguments must be of type T");
        static_assert(sizeof...(Args) == Dim, "Invalid number of arguments");

        i32 i = 0;
        auto f = [this, &i](auto arg) {
            data[i++] = static_cast<T>(arg);
        };
        (f(args), ...);
    }

    // zero vector
    [[nodiscard]] constexpr vec<Dim, T> zero() const {
        vec<Dim, T> ret;
        for (i32 i = 0; i < Dim; ++i) {
            ret[i] = 0;
        }
        return ret;
    }

    // one vector
    [[nodiscard]] constexpr vec<Dim, T> one() const {
        vec<Dim, T> ret;
        for (i32 i = 0; i < Dim; ++i) {
            ret[i] = 1;
        }
        return ret;
    }

    template<i32 D = Dim, typename std::enable_if<(D > 0), int>::type = 0> constexpr T x() { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 0), int>::type = 0> constexpr T r() { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), int>::type = 0> constexpr T y() { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), int>::type = 0> constexpr T g() { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), int>::type = 0> constexpr T z() { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), int>::type = 0> constexpr T b() { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), int>::type = 0> constexpr T w() { return data[3]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), int>::type = 0> constexpr T a() { return data[3]; }

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
                          constexpr void inc()                         { add(1); }

    template<typename U>  constexpr void sub(const vec<Dim, T>& other) { vsub(*this, other); }
    template<typename TA> constexpr void sub(TA v)                     { vsub(*this, v); }
                          constexpr void dec()                         { sub(1); }

    template<typename U>  constexpr void mul(const vec<Dim, T>& other) { vmul(*this, other); }
    template<typename TA> constexpr void mul(TA v)                     { vmul(*this, v); }
    template<typename TA> constexpr void scale(TA v)                   { mul(v); }

    template<typename U>  constexpr void div(const vec<Dim, U>& other) { vdiv(*this, other); }
    template<typename TA> constexpr void div(TA v)                     { vdiv(*this, v); }

    [[nodiscard]] constexpr f64 length() const { return vlength(*this); }

    template<typename U>
    [[nodiscard]] constexpr f64 dot(const vec<Dim, U>& other) const { return vdot(*this, other); }

    template<typename U>
    [[nodiscard]] constexpr vec<Dim, T> cross(const vec<Dim, U>& other) const { return vcross(*this, other); }

    [[nodiscard]] constexpr vec<Dim, T> norm() const { return vnorm(*this); }

    #pragma region overloads "=="

    template<typename U>
    constexpr bool operator==(const vec<Dim, U>& other) const { return equals(other); }

    template<typename U>
    constexpr bool operator!=(const vec<Dim, U>& other) const { return !equals(other); }

    #pragma endregion

    #pragma region overloads negation

    constexpr vec<Dim, T> operator-() const {
        vec<Dim, T> ret = *this;
        ret.negate();
        return ret;
    }

    #pragma endregion

    #pragma region overloads increment and decrement

    constexpr vec<Dim, T>& operator++()   { inc(); return *this; }
    constexpr vec<Dim, T> operator++(int) { vec<Dim, T> ret = *this; inc(); return ret; }
    constexpr vec<Dim, T>& operator--()   { dec(); return *this; }
    constexpr vec<Dim, T> operator--(int) { vec<Dim, T> ret = *this; dec(); return ret; }

    #pragma endregion

    #pragma region overloads assignment operators

    constexpr vec<Dim, T>& operator+=(const vec<Dim, T>& other) { add(other); return *this; }
    constexpr vec<Dim, T>& operator-=(const vec<Dim, T>& other) { sub(other); return *this; }
    constexpr vec<Dim, T>& operator*=(const vec<Dim, T>& other) { mul(other); return *this; }
    constexpr vec<Dim, T>& operator/=(const vec<Dim, T>& other) { div(other); return *this; }

    #pragma endregion

    #pragma region overloads "+"

    [[nodiscard]] friend constexpr vec<Dim, T> operator+(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator+(const vec<Dim, T>& v1, U v2) {
        vec<Dim, T> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator+(U v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v2;
        ret.add(v1);
        return ret;
    }

    #pragma endregion

    #pragma region overloads "-"

    [[nodiscard]] friend constexpr vec<Dim, T> operator-(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator-(const vec<Dim, T>& v1, U v2) {
        vec<Dim, T> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator-(U v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v2;
        ret.sub(v1);
        return ret;
    }

    #pragma endregion

    #pragma region overloads "*"

    [[nodiscard]] friend constexpr vec<Dim, T> operator*(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v1;
        ret.mul(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator*(const vec<Dim, T>& v1, U v2) {
        vec<Dim, T> ret = v1;
        ret.mul(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator*(U v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v2;
        ret.mul(v1);
        return ret;
    }

    #pragma endregion

    #pragma region overloads "/"

    [[nodiscard]] friend constexpr vec<Dim, T> operator/(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v1;
        ret.div(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator/(const vec<Dim, T>& v1, U v2) {
        vec<Dim, T> ret = v1;
        ret.div(v2);
        return ret;
    }
    template<typename U>
    [[nodiscard]] friend constexpr vec<Dim, T> operator/(U v1, const vec<Dim, T>& v2) {
        vec<Dim, T> ret = v2;
        ret.div(v1);
        return ret;
    }

    #pragma endregion
};

template<typename ...Args>
constexpr auto v(Args... args) {
    using commonT = template_args_common_type<Args...>;
    static_assert(template_args_are_of_type_v<commonT, Args...>, "All arguments must be the same type");
    auto ret = vec<sizeof...(Args), commonT>(args...);
    return ret;
}

template<typename T> using vec2 = vec<2, T>;
template<typename T> using vec3 = vec<3, T>;
template<typename T> using vec4 = vec<4, T>;

} // namespace core