#pragma once

#include <types.h>
#include <arr.h>
#include <std/vec.h>
#include <std/mat.h>

#include <type_traits>

namespace core {

using namespace coretypes;

// Translate

template<typename T, i32 Dim>
constexpr void translate(vec<Dim, T>& src, const vec<Dim, T>& t) {
    for (i32 i = 0; i < src.dimensions(); ++i) {
        src[i] += t[i];
    }
}

template<typename T, i32 Dim>
constexpr void translate(arr<vec<Dim, T>>& src, const vec<Dim, T>& t) {
    for (i32 i = 0; i < src.len(); ++i) {
        translate(src[i], t);
    }
}

template<typename T, i32 Dim>
constexpr void translate(mat<Dim + 1, Dim + 1, T>& m, const vec<Dim, T>& t) {
    for (i32 i = 0; i < t.dimensions(); ++i) {
        m[0][i] += t[i];
    }
}

// Scale

template<typename T, i32 Dim>
constexpr void scale(vec<Dim, T>& src, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.dimensions(); ++i) {
        src[i] *= s[i];
    }
}

template<typename T, i32 Dim>
constexpr void scale(vec<Dim, T>& src, const vec<Dim, T>& reference, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.dimensions(); ++i) {
        src[i] = reference[i] + (src[i] - reference[i]) * s[i];
    }
}

template<typename T, i32 Dim>
constexpr void scale(arr<vec<Dim, T>>& src, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.len(); ++i) {
        scale(src[i], s);
    }
}

template<typename T, i32 Dim>
constexpr void scale(arr<vec<Dim, T>>& src, const vec<Dim, T>& reference, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.len(); ++i) {
        scale(src[i], reference, s);
    }
}

template<typename T, i32 Dim>
constexpr void scale(mat<Dim, Dim, T>& m, const vec<Dim, T>& s) {
    for (i32 i = 0; i < m.dimensionsRows(); ++i) {
        m[i][i] *= s[i];
    }
}

template<typename T, i32 Dim>
constexpr void scale(mat<Dim, Dim, T>& m, const vec<Dim, T>& reference, const vec<Dim, T>& s) {
    for (i32 i = 0; i < m.dimensionsRows(); ++i) {
        m[i][i] = reference[i] + (m[i][i] - reference[i]) * s[i];
    }
}

// Rotate

template<typename TFloat>
constexpr void rotate(vec2<TFloat>& src, const vec2<TFloat>& origin, TFloat angle) {
    static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");
    TFloat c = std::cos(angle);
    TFloat s = std::sin(angle);
    src -= origin;
    src = vec2<TFloat>(src.x() * c - src.y() * s, src.x() * s + src.y() * c);
    src += origin;
}

template<typename TFloat>
constexpr void rotate(vec3<TFloat>& src, const vec3<TFloat>& origin, TFloat angle) {
    static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");
    // using 3d rotation anti-clockwise around origin
    // https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    TFloat c = std::cos(angle);
    TFloat s = std::sin(angle);
    TFloat t = 1 - c;

    TFloat x = origin.x();
    TFloat y = origin.y();
    TFloat z = origin.z();

    TFloat x2 = x * x;
    TFloat y2 = y * y;
    TFloat z2 = z * z;

    TFloat xy = x * y;
    TFloat xz = x * z;
    TFloat yz = y * z;

    TFloat xs = x * s;
    TFloat ys = y * s;
    TFloat zs = z * s;

    TFloat m00 = t * x2 + c;
    TFloat m01 = t * xy - zs;
    TFloat m02 = t * xz + ys;
    TFloat m10 = t * xy + zs;
    TFloat m11 = t * y2 + c;
    TFloat m12 = t * yz - xs;
    TFloat m20 = t * xz - ys;
    TFloat m21 = t * yz + xs;
    TFloat m22 = t * z2 + c;

    src = vec3<TFloat>(
        m00 * src.x() + m01 * src.y() + m02 * src.z(),
        m10 * src.x() + m11 * src.y() + m12 * src.z(),
        m20 * src.x() + m21 * src.y() + m22 * src.z()
    );
}

template<typename TFloat>
constexpr void rotate_right(vec2<TFloat>& src, const vec2<TFloat>& origin, TFloat angle) {
    rotate(src, origin, -angle);
}

template<typename TFloat>
constexpr void rotate_right(vec3<TFloat>& src, const vec3<TFloat>& origin, TFloat angle) {
    rotate(src, origin, -angle);
}

template<typename TFloat, i32 Dim>
constexpr void rotate(arr<vec<Dim, TFloat>>& src, const vec<Dim, TFloat>& origin, TFloat angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate(src[i], origin, angle);
    }
}

template<typename TFloat, i32 Dim>
constexpr void rotate_right(arr<vec<Dim, TFloat>>& src, const vec<Dim, TFloat>& origin, TFloat angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate_right(src[i], origin, angle);
    }
}

}
