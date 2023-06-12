#pragma once

#include <types.h>
#include <core_math.h>
#include <std/traits.h>

#include <cmath>

namespace core {

using namespace coretypes;

template<i32 Dim, typename T> struct vec;

#pragma region Static Vector Operations

// Add

template<i32 Dim, typename TDst, typename TSrc>
constexpr void vadd(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] += static_cast<TDst>(src[i]);
    }
}
template<i32 Dim, typename T>
constexpr void vadd(vec<Dim, T>& dst, typename vec<Dim, T>::DataType val) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] += val;
    }
}

// Subtract

template<i32 Dim, typename TDst, typename TSrc>
constexpr void vsub(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] -= static_cast<TDst>(src[i]);
    }
}
template<i32 Dim, typename T>
constexpr void vsub(vec<Dim, T>& dst, typename vec<Dim, T>::DataType val) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] -= val;
    }
}

// Multiplication

template<i32 Dim, typename TDst, typename TSrc>
constexpr void vmul(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] *= static_cast<TDst>(src[i]);
    }
}
template<i32 Dim, typename T>
constexpr void vmul(vec<Dim, T>& dst, typename vec<Dim, T>::DataType val) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        dst[i] *= val;
    }
}

// Division

template<i32 Dim, typename TDst, typename TSrc>
constexpr void vdiv(vec<Dim, TDst>& dst, const vec<Dim, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        Assert(static_cast<TDst>(src[i]) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
        dst[i] /= static_cast<TDst>(src[i]);
    }
}
template<i32 Dim, typename T>
constexpr void vdiv(vec<Dim, T>& dst, typename vec<Dim, T>::DataType val) {
    for (i32 i = 0; i < dst.dimensions(); ++i) {
        Assert(static_cast<T>(val) != 0, "Division by zero"); // TODO: this assert is costly. Should have an assert that only runs in debug mode.
        dst[i] /= static_cast<T>(val);
    }
}

// Length

template<i32 Dim, typename T>
constexpr f64 vlengthsq(const vec<Dim, T>& v) {
    f64 ret = 0;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret += static_cast<f64>(v[i]) * static_cast<f64>(v[i]);
    }
    return ret;
}

template<i32 Dim, typename T>
constexpr f64 vlength(const vec<Dim, T>& v) {
    f64 ret = vlengthsq(v);
    ret = std::sqrt(ret);
    return ret;
}

// Negation

template<i32 Dim, typename T>
constexpr void vnegate(vec<Dim, T>& v) {
    for (i32 i = 0; i < v.dimensions(); ++i) {
        v[i] = -v[i];
    }
}

// Dot Product

template<i32 Dim, typename T>
constexpr f64 vdot(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
    f64 ret = 0;
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret += v1[i] * v2[i];
    }
    return ret;
}

// Cross Product

template<i32 Dim, typename T>
constexpr vec<Dim, T> vcross(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
    vec<Dim, T> ret;

    // Modulo operator is really slow. So for vectors of the most used dimensions the dot product is written by hand:
    if constexpr (Dim == 3) {
        ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
        ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
        ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
        return ret;
    }

    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret[i] = v1[(i + 1) % v1.dimensions()] * v2[(i + 2) % v1.dimensions()] -
                 v1[(i + 2) % v1.dimensions()] * v2[(i + 1) % v1.dimensions()];
    }
    return ret;
}

// Equality

template<i32 Dim, typename T>
constexpr bool vequals(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        if (v1[i] != v2[i]) return false;
    }
    return true;
}
template<i32 Dim, typename T>
constexpr bool vsafeequals(const vec<Dim, T>& v1, const vec<Dim, T>& v2, T epsilon) {
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        if (!core::safe_eq(v1[i], v2[i], epsilon)) return false;
    }
    return true;
}

// Uniform

template<i32 Dim, typename T>
static constexpr vec<Dim, T> uniform(typename vec<Dim, T>::DataType val) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < Dim; ++i) {
        ret[i] = val;
    }
    return ret;
}

// Zero Vector

template<i32 Dim, typename T>
constexpr vec<Dim, T> vzero() {
    vec<Dim, T> ret;
    for (i32 i = 0; i < Dim; ++i) {
        ret[i] = 0;
    }
    return ret;
}

// One Vector

template<i32 Dim, typename T>
constexpr vec<Dim, T> vone() {
    vec<Dim, T> ret;
    for (i32 i = 0; i < Dim; ++i) {
        ret[i] = 1;
    }
    return ret;
}

// Normalization

template<i32 Dim, typename T>
constexpr vec<Dim, T> vnorm(const vec<Dim, T>& v) {
    f64 len = vlength(v);
    if (len == 0) return vzero<Dim, T>();
    vec<Dim, T> ret = v;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] /= static_cast<T>(len);
    }
    return ret;
}

// Absolute Value

template<i32 Dim, typename T>
constexpr vec<Dim, T> vabs(const vec<Dim, T>& v) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] = core::abs(v[i]);
    }
    return ret;
}

// Round Value
template<i32 Dim, typename T>
constexpr vec<Dim, T> vround(const vec<Dim, T>& v) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] = core::round(v[i]);
    }
    return ret;
}

// Max Value

template<i32 Dim, typename T>
constexpr vec<Dim, T> vmax(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret[i] = core::max(v1[i], v2[i]);
    }
    return ret;
}

// Min Value

template<i32 Dim, typename T>
constexpr vec<Dim, T> vmin(const vec<Dim, T>& v1, const vec<Dim, T>& v2) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v1.dimensions(); ++i) {
        ret[i] = core::min(v1[i], v2[i]);
    }
    return ret;
}

// Ceiling

template<i32 Dim, typename T>
constexpr vec<Dim, T> vceil(const vec<Dim, T>& v) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] = core::ceil(v[i]);
    }
    return ret;
}

// Floor

template<i32 Dim, typename T>
constexpr vec<Dim, T> vfloor(const vec<Dim, T>& v) {
    vec<Dim, T> ret;
    for (i32 i = 0; i < v.dimensions(); ++i) {
        ret[i] = core::floor(v[i]);
    }
    return ret;
}

#pragma endregion

template<i32 Dim, typename T>
struct vec {
    using DataType = T;
    static_assert(std::is_trivial_v<DataType>, "DataType must be trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be arithmetic Type");
    static_assert(Dim > 0, "Dim must be greater than 0");

    static constexpr i32 dimensions() { return Dim; }

    DataType data[Dim] = {}; // initializing to zero allows use in constexpr.. c++ nonsense

    constexpr vec() = default;

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

    template<typename U> constexpr void add(const vec<Dim, U>& other) { vadd(*this, other); }
                         constexpr void add(DataType v)               { vadd(*this, v); }
                         constexpr void inc()                         { add(DataType(1)); }

    template<typename U> constexpr void sub(const vec<Dim, U>& other) { vsub(*this, other); }
                         constexpr void sub(DataType v)               { vsub(*this, v); }
                         constexpr void dec()                         { sub(DataType(1)); }

    template<typename U> constexpr void mul(const vec<Dim, U>& other) { vmul(*this, other); }
                         constexpr void mul(DataType v)               { vmul(*this, v); }
                         constexpr void scale(DataType v)             { mul(v); }

    template<typename U> constexpr void div(const vec<Dim, U>& other) { vdiv(*this, other); }
                         constexpr void div(DataType v)               { vdiv(*this, v); }

    constexpr f64 length() const { return vlength(*this); }

    constexpr f64 dot(const vec<Dim, DataType>& other) const { return vdot(*this, other); }

    constexpr vec<Dim, DataType> cross(const vec<Dim, DataType>& other) const { return vcross(*this, other); }

    constexpr vec<Dim, DataType> norm() const { return vnorm(*this); }

    constexpr bool operator==(const vec<Dim, DataType>& other) const { return equals(other); }
    constexpr bool operator!=(const vec<Dim, DataType>& other) const { return !equals(other); }

    constexpr vec<Dim, DataType> operator-() const {
        vec<Dim, DataType> ret = *this;
        ret.negate();
        return ret;
    }

    constexpr vec<Dim, DataType>& operator++()   { inc(); return *this; }
    constexpr vec<Dim, DataType> operator++(i32) { vec<Dim, DataType> ret = *this; inc(); return ret; }
    constexpr vec<Dim, DataType>& operator--()   { dec(); return *this; }
    constexpr vec<Dim, DataType> operator--(i32) { vec<Dim, DataType> ret = *this; dec(); return ret; }

    constexpr vec<Dim, DataType>& operator+=(const vec<Dim, DataType>& other) { add(other); return *this; }
    constexpr vec<Dim, DataType>& operator+=(DataType v)                      { add(v); return *this; }
    constexpr vec<Dim, DataType>& operator-=(const vec<Dim, DataType>& other) { sub(other); return *this; }
    constexpr vec<Dim, DataType>& operator-=(DataType v)                      { sub(v); return *this; }
    constexpr vec<Dim, DataType>& operator*=(const vec<Dim, DataType>& other) { mul(other); return *this; }
    constexpr vec<Dim, DataType>& operator*=(DataType v)                      { mul(v); return *this; }
    constexpr vec<Dim, DataType>& operator/=(const vec<Dim, DataType>& other) { div(other); return *this; }
    constexpr vec<Dim, DataType>& operator/=(DataType v)                      { div(v); return *this; }
};

// Operators

#pragma region overloads "+"

template<i32 Dim, typename T>
constexpr vec<Dim, T> operator+(const vec<Dim, T>& lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = lhs;
    ret.add(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator+(const vec<Dim, T>& lhs, typename vec<Dim, T>::DataType rhs) {
    vec<Dim, T> ret = lhs;
    ret.add(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator+(typename vec<Dim, T>::DataType lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = rhs;
    ret.add(lhs);
    return ret;
}

#pragma endregion

#pragma region overloads "-"

template<i32 Dim, typename T>
constexpr vec<Dim, T> operator-(const vec<Dim, T>& lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = lhs;
    ret.sub(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator-(const vec<Dim, T>& lhs, typename vec<Dim, T>::DataType rhs) {
    vec<Dim, T> ret = lhs;
    ret.sub(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator-(typename vec<Dim, T>::DataType lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = rhs;
    for (i32 i = 0; i < ret.dimensions(); ++i) {
        ret[i] = -(ret[i] - static_cast<T>(lhs));
    }
    return ret;
}

#pragma endregion

#pragma region overloads "*"

template<i32 Dim, typename T>
constexpr vec<Dim, T> operator*(const vec<Dim, T>& lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = lhs;
    ret.mul(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator*(const vec<Dim, T>& lhs, typename vec<Dim, T>::DataType rhs) {
    vec<Dim, T> ret = lhs;
    ret.mul(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator*(typename vec<Dim, T>::DataType lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = rhs;
    ret.mul(lhs);
    return ret;
}

#pragma endregion

#pragma region overloads "/"

template<i32 Dim, typename T>
constexpr vec<Dim, T> operator/(const vec<Dim, T>& lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = lhs;
    ret.div(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator/(const vec<Dim, T>& lhs, typename vec<Dim, T>::DataType rhs) {
    vec<Dim, T> ret = lhs;
    ret.div(rhs);
    return ret;
}
template<i32 Dim, typename T>
constexpr vec<Dim, T> operator/(typename vec<Dim, T>::DataType lhs, const vec<Dim, T>& rhs) {
    vec<Dim, T> ret = rhs;
    for (i32 i = 0; i < ret.dimensions(); ++i) {
        ret[i] = static_cast<T>(lhs) / ret[i];
    }
    return ret;
}

#pragma endregion

// Factory

template<typename ...Args>
constexpr auto v(Args... args) {
    using commonT = args_common_type<Args...>;
    static_assert(args_have_common_type_v<commonT, Args...>, "All arguments must be the same Type");

    // The absolute magic of c++ templates. What where they somking when they came up with this syntax?
    vec<sizeof...(Args), commonT> ret;
    i32 i = 0;
    auto f = [&](auto arg) { ret.data[i++] = static_cast<decltype(arg)>(arg); };
    (f(args), ...);

    return ret;
}

// Converter

template<typename TVec, typename TVec2>
constexpr TVec v_conv(const TVec2& v) {
    TVec ret;
    for (i32 i = 0; i < TVec2::dimensions(); i++) {
        ret[i] = static_cast<typename TVec2::DataType>(v[i]);
    }
    return ret;
}

// Definitions

template<typename T> using vec1 = vec<1, T>;
template<typename T> using vec2 = vec<2, T>;
template<typename T> using vec3 = vec<3, T>;
template<typename T> using vec4 = vec<4, T>;

using vec1b = vec1<bool>;
using vec2b = vec2<bool>;
using vec3b = vec3<bool>;
using vec4b = vec4<bool>;

using vec1i = vec1<i32>;
using vec2i = vec2<i32>;
using vec3i = vec3<i32>;
using vec4i = vec4<i32>;

using vec1u = vec1<u32>;
using vec2u = vec2<u32>;
using vec3u = vec3<u32>;
using vec4u = vec4<u32>;

using vec1f = vec1<f32>;
using vec2f = vec2<f32>;
using vec3f = vec3<f32>;
using vec4f = vec4<f32>;

using vec1d = vec1<f64>;
using vec2d = vec2<f64>;
using vec3d = vec3<f64>;
using vec4d = vec4<f64>;

constexpr static vec4f WHITE   = v(1.0f, 1.0f, 1.0f, 1.0f);
constexpr static vec4f BLACK   = v(0.0f, 0.0f, 0.0f, 1.0f);
constexpr static vec4f RED     = v(1.0f, 0.0f, 0.0f, 1.0f);
constexpr static vec4f GREEN   = v(0.0f, 1.0f, 0.0f, 1.0f);
constexpr static vec4f BLUE    = v(0.0f, 0.0f, 1.0f, 1.0f);
constexpr static vec4f YELLOW  = v(1.0f, 1.0f, 0.0f, 1.0f);
constexpr static vec4f MAGENTA = v(1.0f, 0.0f, 1.0f, 1.0f);
constexpr static vec4f CYAN    = v(0.0f, 1.0f, 1.0f, 1.0f);

} // namespace core
