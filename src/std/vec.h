#pragma once

#include <types.h>
#include <std/traits.h>

#include <cmath>

namespace core {

using namespace coretypes;

template<i32 Dim, typename T> struct vec;

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vadd(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] += static_cast<TDst>(src[i]);
    }
}

template<typename TDst, i32 Dim, typename TVal>
constexpr void vadd(vec<Dim, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be arithmetic");
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] += static_cast<TDst>(val);
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vsub(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] -= static_cast<TDst>(src[i]);
    }
}

template<typename TDst, i32 Dim, typename TVal>
constexpr void vsub(vec<Dim, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be arithmetic");
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] -= static_cast<TDst>(val);
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vmul(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] *= static_cast<TDst>(src[i]);
    }
}

template<typename TDst, i32 Dim, typename TVal>
constexpr void vmul(vec<Dim, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be arithmetic");
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] *= static_cast<TDst>(val);
    }
}

template<typename TDst, i32 Dim, typename TSrc>
constexpr void vdiv(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        Assert(static_cast<TDst>(src[i]) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
        dst[i] /= static_cast<TDst>(src[i]);
    }
}

template<typename TDst, i32 Dim, typename TVal>
constexpr void vdiv(vec<Dim, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be arithmetic");
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        Assert(static_cast<TDst>(val) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
        dst[i] /= static_cast<TDst>(val);
    }
}

template<typename T, i32 Dim>
constexpr f64 vlength(const vec<Dim, T>& v) {
    f64 ret = 0;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret += static_cast<f64>(v[i]) * static_cast<f64>(v[i]);
    }
    ret = std::sqrt(ret);
    return ret;
}

template<typename T, i32 Dim>
constexpr void vnegate(vec<Dim, T>& v) {
    for (i32 i = 0; i < v.dimensions(); ++i) {
        v[i] = -v[i];
    }
}

template<typename T1, typename T2, i32 Dim>
constexpr f64 vdot(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    f64 ret = 0;
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret += v1[i] * static_cast<T1>(v2[i]);
    }
    return ret;
}

template<typename T1, typename T2, i32 Dim>
constexpr vec<Dim, T1> vcross(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    vec<Dim, T1> ret;
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret[i] = v1[(i + 1) % v1.dimensions()] * v2[(i + 2) % v1.dimensions()] -
                 v1[(i + 2) % v1.dimensions()] * v2[(i + 1) % v1.dimensions()];
    }
    return ret;
}

template<typename T1, typename T2, i32 Dim>
constexpr bool vequals(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2) {
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        if (v1[i] != static_cast<T1>(v2[i])) return false;
    }
    return true;
}

template<typename T1, typename T2, i32 Dim>
constexpr bool vsafeequals(const vec<Dim, T1>& v1, const vec<Dim, T2>& v2, T1 epsilon) {
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        if (!core::safe_eq(v1[i], static_cast<T1>(v2[i]), epsilon)) return false;
    }
    return true;
}

template<typename T, i32 Dim>
constexpr vec<Dim, T> vnorm(const vec<Dim, T>& v) {
    f64 len = vlength(v);
    if (len == 0) return v.zero();
    vec<Dim, T> ret = v;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] /= static_cast<T>(len);
    }
    return ret;
}

template<i32 Dim, typename T>
struct vec {
    using DataType = T;
    static_assert(std::is_trivial_v<DataType>, "DataType must be trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be arithmetic Type");
    static_assert(Dim > 0, "Dim must be greater than 0");

    static constexpr i32 dimensions() { return Dim; }
    static constexpr vec<Dim, DataType> uniform(DataType val) {
        vec<Dim, DataType> ret;
        core::memset(ret.data, val, sizeof(DataType) * Dim);
        return ret;
    }

    DataType data[Dim] = {}; // initializing to zero allows use in constexpr.. c++ nonsense

    constexpr vec() = default;

    template<typename ...Args>
    constexpr vec(Args... args) {
        using commonT = template_args_common_type<Args...>;
        static_assert(std::is_same_v<commonT, DataType>, "All arguments must be the same DataType");
        static_assert(sizeof...(Args) == Dim, "Invalid number of arguments");

        i32 i = 0;
        auto f = [this, &i](auto arg) {
            data[i++] = static_cast<DataType>(arg);
        };
        (f(args), ...);
    }

    // zero vector
    constexpr vec<Dim, DataType> zero() const {
        vec<Dim, DataType> ret;
        for (i32 i = 0; i < Dim; ++i) {
            ret[i] = 0;
        }
        return ret;
    }

    // one vector
    constexpr vec<Dim, DataType> one() const {
        vec<Dim, DataType> ret;
        for (i32 i = 0; i < Dim; ++i) {
            ret[i] = 1;
        }
        return ret;
    }

    template<i32 D = Dim, typename std::enable_if<(D > 0), i32>::type = 0> constexpr DataType& x() { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 0), i32>::type = 0> constexpr DataType& r() { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), i32>::type = 0> constexpr DataType& y() { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), i32>::type = 0> constexpr DataType& g() { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), i32>::type = 0> constexpr DataType& z() { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), i32>::type = 0> constexpr DataType& b() { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), i32>::type = 0> constexpr DataType& w() { return data[3]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), i32>::type = 0> constexpr DataType& a() { return data[3]; }

    template<i32 D = Dim, typename std::enable_if<(D > 0), i32>::type = 0> constexpr const DataType& x() const { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 0), i32>::type = 0> constexpr const DataType& r() const { return data[0]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), i32>::type = 0> constexpr const DataType& y() const { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 1), i32>::type = 0> constexpr const DataType& g() const { return data[1]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), i32>::type = 0> constexpr const DataType& z() const { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 2), i32>::type = 0> constexpr const DataType& b() const { return data[2]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), i32>::type = 0> constexpr const DataType& w() const { return data[3]; }
    template<i32 D = Dim, typename std::enable_if<(D > 3), i32>::type = 0> constexpr const DataType& a() const { return data[3]; }

    constexpr DataType& operator[](i32 i) {
        Assert(i >= 0 && i < Dim, "Index out of bounds");
        return data[i];
    }
    constexpr const DataType& operator[](i32 i) const {
        Assert(i >= 0 && i < Dim, "Index out of bounds");
        return data[i];
    }

    constexpr bool equals(const vec<Dim, DataType>& other) const                   { return vequals(*this, other); }
    constexpr bool equals(const vec<Dim, DataType>& other, DataType epsilon) const { return vsafeequals(*this, other, epsilon); }

    constexpr void negate() { vnegate(*this); }

    template<typename U>  constexpr void add(const vec<Dim, U>& other) { vadd(*this, other); }
    template<typename TA> constexpr void add(TA v)                     { vadd(*this, v); }
                          constexpr void inc()                         { add(1); }

    template<typename U>  constexpr void sub(const vec<Dim, U>& other) { vsub(*this, other); }
    template<typename TA> constexpr void sub(TA v)                     { vsub(*this, v); }
                          constexpr void dec()                         { sub(1); }

    template<typename U>  constexpr void mul(const vec<Dim, U>& other) { vmul(*this, other); }
    template<typename TA> constexpr void mul(TA v)                     { vmul(*this, v); }
    template<typename TA> constexpr void scale(TA v)                   { mul(v); }

    template<typename U>  constexpr void div(const vec<Dim, U>& other) { vdiv(*this, other); }
    template<typename TA> constexpr void div(TA v)                     { vdiv(*this, v); }

    constexpr f64 length() const { return vlength(*this); }

    template<typename U>
    constexpr f64 dot(const vec<Dim, U>& other) const { return vdot(*this, other); }

    template<typename U>
    constexpr vec<Dim, DataType> cross(const vec<Dim, U>& other) const { return vcross(*this, other); }

    constexpr vec<Dim, DataType> norm() const { return vnorm(*this); }

#pragma region overloads "=="

    constexpr bool operator==(const vec<Dim, DataType>& other) const { return equals(other); }
    constexpr bool operator!=(const vec<Dim, DataType>& other) const { return !equals(other); }

#pragma endregion

#pragma region overloads negation

    constexpr vec<Dim, DataType> operator-() const {
        vec<Dim, DataType> ret = *this;
        ret.negate();
        return ret;
    }

#pragma endregion

#pragma region overloads increment and decrement

    constexpr vec<Dim, DataType>& operator++()   { inc(); return *this; }
    constexpr vec<Dim, DataType> operator++(i32) { vec<Dim, DataType> ret = *this; inc(); return ret; }
    constexpr vec<Dim, DataType>& operator--()   { dec(); return *this; }
    constexpr vec<Dim, DataType> operator--(i32) { vec<Dim, DataType> ret = *this; dec(); return ret; }

#pragma endregion

#pragma region overloads assignment operators

    constexpr vec<Dim, DataType>& operator+=(const vec<Dim, DataType>& other) { add(other); return *this; }
    constexpr vec<Dim, DataType>& operator+=(DataType v)                      { add(v); return *this; }
    constexpr vec<Dim, DataType>& operator-=(const vec<Dim, DataType>& other) { sub(other); return *this; }
    constexpr vec<Dim, DataType>& operator-=(DataType v)                      { sub(v); return *this; }
    constexpr vec<Dim, DataType>& operator*=(const vec<Dim, DataType>& other) { mul(other); return *this; }
    constexpr vec<Dim, DataType>& operator*=(DataType v)                      { mul(v); return *this; }
    constexpr vec<Dim, DataType>& operator/=(const vec<Dim, DataType>& other) { div(other); return *this; }
    constexpr vec<Dim, DataType>& operator/=(DataType v)                      { div(v); return *this; }

#pragma endregion

#pragma region overloads "+"

    friend constexpr vec<Dim, DataType> operator+(const vec<Dim, DataType>& v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator+(const vec<Dim, DataType>& v1, U v2) {
        vec<Dim, DataType> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator+(U v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v2;
        ret.add(v1);
        return ret;
    }

#pragma endregion

#pragma region overloads "-"

    friend constexpr vec<Dim, DataType> operator-(const vec<Dim, DataType>& v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator-(const vec<Dim, DataType>& v1, U v2) {
        vec<Dim, DataType> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator-(U v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v2;
        for (i32 i = 0; i < ret.dimensions(); ++i) {
            ret[i] = -(ret[i] - static_cast<DataType>(v1));
        }
        return ret;
    }

#pragma endregion

#pragma region overloads "*"

    friend constexpr vec<Dim, DataType> operator*(const vec<Dim, DataType>& v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v1;
        ret.mul(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator*(const vec<Dim, DataType>& v1, U v2) {
        vec<Dim, DataType> ret = v1;
        ret.mul(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator*(U v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v2;
        ret.mul(v1);
        return ret;
    }

#pragma endregion

#pragma region overloads "/"

    friend constexpr vec<Dim, DataType> operator/(const vec<Dim, DataType>& v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v1;
        ret.div(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator/(const vec<Dim, DataType>& v1, U v2) {
        vec<Dim, DataType> ret = v1;
        ret.div(v2);
        return ret;
    }
    template<typename U>
    friend constexpr vec<Dim, DataType> operator/(U v1, const vec<Dim, DataType>& v2) {
        vec<Dim, DataType> ret = v2;
        for (i32 i = 0; i < ret.dimensions(); ++i) {
            ret[i] = static_cast<DataType>(v1) / ret[i];
        }
        return ret;
    }

#pragma endregion

};

template<typename ...Args>
constexpr auto v(Args... args) {
    using commonT = template_args_common_type<Args...>;
    static_assert(template_args_are_of_type_v<commonT, Args...>, "All arguments must be the same Type");
    auto ret = vec<sizeof...(Args), commonT>(args...);
    return ret;
}

template<typename TVec, typename TVec2>
constexpr TVec v_conv(const TVec2& v) {
    TVec ret;
    for (i32 i = 0; i < TVec2::dimensions(); i++) {
        ret[i] = static_cast<typename TVec2::DataType>(v[i]);
    }
    return ret;
}

template<typename T> using vec2 = vec<2, T>;
template<typename T> using vec3 = vec<3, T>;
template<typename T> using vec4 = vec<4, T>;

using vec2b = vec2<bool>;
using vec3b = vec3<bool>;
using vec4b = vec4<bool>;

using vec2i = vec2<i32>;
using vec3i = vec3<i32>;
using vec4i = vec4<i32>;

using vec2u = vec2<u32>;
using vec3u = vec3<u32>;
using vec4u = vec4<u32>;

using vec2f = vec2<f32>;
using vec3f = vec3<f32>;
using vec4f = vec4<f32>;

using vec2d = vec2<f64>;
using vec3d = vec3<f64>;
using vec4d = vec4<f64>;

} // namespace core
