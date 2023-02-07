#pragma once

#include <types.h>
#include <arr.h>
#include <std/vec.h>

namespace core {

using namespace coretypes;

// TODO: Implement versions that use a matrix.

template<typename T, i32 Dim>
constexpr void translate(vec<Dim, T>& src, const vec<Dim, T>& t) {
    for (i32 i = 0; i < src.dimentions(); ++i) {
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
constexpr void scale(vec<Dim, T>& src, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.dimentions(); ++i) {
        src[i] *= s[i];
    }
}

template<typename T, i32 Dim>
constexpr void scale(vec<Dim, T>& src, const vec<Dim, T>& reference, const vec<Dim, T>& s) {
    for (i32 i = 0; i < src.dimentions(); ++i) {
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

template<typename T>
constexpr void rotate(vec2<T>& src, const vec2<T>& axis, double angle) {
    T c = cos(angle);
    T s = sin(angle);
    src -= axis;
    src = vec2<T>(src.x() * c - src.y() * s, src.x() * s + src.y() * c);
    src += axis;
}

template<typename T>
constexpr void rotate(vec3<T>& src, const vec3<T>& axis, double angle) {
    // using 3d rotation anti-clockwise around axis
    // https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    T c = cos(angle);
    T s = sin(angle);
    T t = 1 - c;

    T x = axis.x();
    T y = axis.y();
    T z = axis.z();

    T x2 = x * x;
    T y2 = y * y;
    T z2 = z * z;

    T xy = x * y;
    T xz = x * z;
    T yz = y * z;

    T xs = x * s;
    T ys = y * s;
    T zs = z * s;

    T m00 = t * x2 + c;
    T m01 = t * xy - zs;
    T m02 = t * xz + ys;
    T m10 = t * xy + zs;
    T m11 = t * y2 + c;
    T m12 = t * yz - xs;
    T m20 = t * xz - ys;
    T m21 = t * yz + xs;
    T m22 = t * z2 + c;

    src = vec3<T>(
        m00 * src.x() + m01 * src.y() + m02 * src.z(),
        m10 * src.x() + m11 * src.y() + m12 * src.z(),
        m20 * src.x() + m21 * src.y() + m22 * src.z()
    );
}

template<typename T>
constexpr void rotate_right(vec2<T>& src, const vec2<T>& axis, double angle) {
    rotate(src, axis, -angle);
}

template<typename T>
constexpr void rotate_right(vec3<T>& src, const vec3<T>& axis, double angle) {
    rotate(src, axis, -angle);
}

template<typename T, i32 Dim>
constexpr void rotate(arr<vec<Dim, T>>& src, const vec<Dim, T>& axis, double angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate(src[i], axis, angle);
    }
}

template<typename T, i32 Dim>
constexpr void rotate_right(arr<vec<Dim, T>>& src, const vec<Dim, T>& axis, double angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate_right(src[i], axis, angle);
    }
}

}
