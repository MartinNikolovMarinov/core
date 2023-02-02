#pragma once

#include <types.h>
#include <arr.h>
#include <std/vec.h>

namespace core {

using namespace coretypes;

// TODO: Use matrix for all of these.

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
constexpr void rotate(vec2<T>& src, const vec2<T>& axis, const T& angle) {
    T c = cos(angle);
    T s = sin(angle);
    src -= axis;
    src = vec2<T>(src.x() * c - src.y() * s, src.x() * s + src.y() * c);
    src += axis;
}

// TODO: implement 3d and 4d rotations at some point.

template<typename T>
constexpr void rotate_right(vec2<T>& src, const vec2<T>& axis, const T& angle) {
    rotate(src, axis, -angle);
}

template<typename T, i32 Dim>
constexpr void rotate(arr<vec<Dim, T>>& src, const vec<Dim, T>& axis, const T& angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate(src[i], axis, angle);
    }
}

template<typename T, i32 Dim>
constexpr void rotate_right(arr<vec<Dim, T>>& src, const vec<Dim, T>& axis, const T& angle) {
    for (i32 i = 0; i < src.len(); ++i) {
        rotate_right(src[i], axis, angle);
    }
}

}
