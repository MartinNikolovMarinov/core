#pragma once

#include <types.h>
#include <arr.h>
#include <std/vec.h>
#include <std/mat.h>

#include <type_traits>
#include <cmath>

// FIXME: Create a concrete type for radians! The api should make it obvious that radians are used.

namespace core {

using namespace coretypes;

// Translate

template<i32 Dim, typename T>
constexpr void translate(vec<Dim, T>& v, const vec<Dim, T>& t) {
    for (i32 i = 0; i < v.dimensions(); ++i) {
        v[i] += t[i];
    }
}

template<i32 Dim, typename T>
constexpr void translate(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& t) {
    for (i32 i = 0; i < arrv.len(); ++i) {
        translate(arrv[i], t);
    }
}

template<typename T>
constexpr void translate(mat4x4<T>& m, const vec3<T>& t) {
    // Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];

    m[3][0] += m[0][0] * t[0] + m[1][0] * t[1] + m[2][0] * t[2];
    m[3][1] += m[0][1] * t[0] + m[1][1] * t[1] + m[2][1] * t[2];
    m[3][2] += m[0][2] * t[0] + m[1][2] * t[1] + m[2][2] * t[2];
    m[3][3] += m[0][3] * t[0] + m[1][3] * t[1] + m[2][3] * t[2];
}

// Scale

template<i32 Dim, typename T>
constexpr void scale(vec<Dim, T>& v, const vec<Dim, T>& s) {
    for (i32 i = 0; i < v.dimensions(); ++i) {
        v[i] *= s[i];
    }
}

template<i32 Dim, typename T>
constexpr void scale(vec<Dim, T>& v, const vec<Dim, T>& axis, const vec<Dim, T>& s) {
    for (i32 i = 0; i < v.dimensions(); ++i) {
        v[i] = axis[i] + (v[i] - axis[i]) * s[i];
    }
}

template<i32 Dim, typename T>
constexpr void scale(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& s) {
    for (i32 i = 0; i < arrv.len(); ++i) {
        scale(arrv[i], s);
    }
}

template<i32 Dim, typename T>
constexpr void scale(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& axis, const vec<Dim, T>& s) {
    for (i32 i = 0; i < arrv.len(); ++i) {
        scale(arrv[i], axis, s);
    }
}

template<typename T>
constexpr void scale(mat4x4<T>& m, const vec3<T>& s) {
    m[0][0] *= s[0]; m[0][1] *= s[0]; m[0][2] *= s[0];
    m[1][0] *= s[1]; m[1][1] *= s[1]; m[1][2] *= s[1];
    m[2][0] *= s[2]; m[2][1] *= s[2]; m[2][2] *= s[2];
}

// Rotate

template<typename TFloat>
constexpr void rotate(vec2<TFloat>& v, const vec2<TFloat>& origin, TFloat angle) {
    static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");
    TFloat c = std::cos(angle);
    TFloat s = std::sin(angle);
    v -= origin;
    v = core::v<TFloat>(v.x() * c - v.y() * s, v.x() * s + v.y() * c);
    v += origin;
}

template<typename TFloat, i32 Dim>
constexpr void rotate(arr<vec<Dim, TFloat>>& v, const vec<Dim, TFloat>& axis, TFloat angle) {
    for (i32 i = 0; i < v.len(); ++i) {
        rotate(v[i], axis, angle);
    }
}

template<typename TFloat>
constexpr void rotate(mat4x4<TFloat>& m, const vec3<TFloat>& v, TFloat angle) {
    static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");

    TFloat c = std::cos(angle);
    TFloat s = std::sin(angle);
    TFloat t = TFloat(1) - c;

    vec3<TFloat> axis = v.norm();

    auto m00 = t * axis.x() * axis.x() + c;
    auto m10 = t * axis.x() * axis.y() - s * axis.z();
    auto m20 = t * axis.x() * axis.z() + s * axis.y();
    auto m01 = t * axis.x() * axis.y() + s * axis.z();
    auto m11 = t * axis.y() * axis.y() + c;
    auto m21 = t * axis.y() * axis.z() - s * axis.x();
    auto m02 = t * axis.x() * axis.z() - s * axis.y();
    auto m12 = t * axis.y() * axis.z() + s * axis.x();
    auto m22 = t * axis.z() * axis.z() + c;

    core::mat4<TFloat> rotate (
        m00, m10, m20, 0,
        m01, m11, m21, 0,
        m02, m12, m22, 0,
          0,   0,   0, 1
    );

    m = rotate * m;
}


template<typename TFloat>
constexpr void rotate_right(vec2<TFloat>& v, const vec2<TFloat>& axis, TFloat angle) {
    rotate(v, axis, -angle);
}

template<typename TFloat, i32 Dim>
constexpr void rotate_right(arr<vec<Dim, TFloat>>& v, const vec<Dim, TFloat>& axis, TFloat angle) {
    for (i32 i = 0; i < v.len(); ++i) {
        rotate_right(v[i], axis, angle);
    }
}

template<typename TFloat>
constexpr void rotate_right(mat4x4<TFloat>& v, const vec3<TFloat>& axis, TFloat angle) {
    rotate(v, axis, -angle);
}

}
