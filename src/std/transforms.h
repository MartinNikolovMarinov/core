#pragma once

#include <types.h>
#include <arr.h>
#include <std/vec.h>
#include <std/mat.h>

#include <type_traits>

// FIXME: Create a concrete type for radians! The api should make it obvious that radians are used.

namespace core {

using namespace coretypes;

// Translate

// template<i32 Dim, typename T>
// constexpr void translate(vec<Dim, T>& v, const vec<Dim, T>& t) {
//     for (i32 i = 0; i < v.dimensions(); ++i) {
//         v[i] += t[i];
//     }
// }

// template<i32 Dim, typename T>
// constexpr void translate(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& t) {
//     for (i32 i = 0; i < arrv.len(); ++i) {
//         translate(arrv[i], t);
//     }
// }

// template<typename T>
// constexpr void translate(mat4x4<T>& m, const vec3<T>& t) {
//     m[3] = m[0] * t[0] + m[1] * t[1] + m[2] * t[2] + m[3];
// }

// // Scale

// template<i32 Dim, typename T>
// constexpr void scale(vec<Dim, T>& v, const vec<Dim, T>& s) {
//     for (i32 i = 0; i < v.dimensions(); ++i) {
//         v[i] *= s[i];
//     }
// }

// template<i32 Dim, typename T>
// constexpr void scale(vec<Dim, T>& v, const vec<Dim, T>& axis, const vec<Dim, T>& s) {
//     for (i32 i = 0; i < v.dimensions(); ++i) {
//         v[i] = axis[i] + (v[i] - axis[i]) * s[i];
//     }
// }

// template<i32 Dim, typename T>
// constexpr void scale(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& s) {
//     for (i32 i = 0; i < arrv.len(); ++i) {
//         scale(arrv[i], s);
//     }
// }

// template<i32 Dim, typename T>
// constexpr void scale(arr<vec<Dim, T>>& arrv, const vec<Dim, T>& axis, const vec<Dim, T>& s) {
//     for (i32 i = 0; i < arrv.len(); ++i) {
//         scale(arrv[i], axis, s);
//     }
// }

// template<typename T>
// constexpr void scale(mat4x4<T>& m, const vec3<T>& s) {
//     m[0] *= s[0];
//     m[1] *= s[1];
//     m[2] *= s[2];
// }

// // Rotate

// template<typename TFloat>
// constexpr void rotate(vec2<TFloat>& v, const vec2<TFloat>& origin, TFloat angle) {
//     static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");
//     TFloat c = std::cos(angle);
//     TFloat s = std::sin(angle);
//     v -= origin;
//     v = core::v<TFloat>(v.x() * c - v.y() * s, v.x() * s + v.y() * c);
//     v += origin;
// }

// template<typename TFloat, i32 Dim>
// constexpr void rotate(arr<vec<Dim, TFloat>>& v, const vec<Dim, TFloat>& axis, TFloat angle) {
//     for (i32 i = 0; i < v.len(); ++i) {
//         rotate(v[i], axis, angle);
//     }
// }

// template<typename TFloat>
// constexpr void rotate(mat4x4<TFloat>& m, const vec3<TFloat>& axis, TFloat angle) {
//     static_assert(std::is_floating_point_v<TFloat>, "type must be floating point");

//     TFloat c = std::cos(angle);
//     TFloat s = std::sin(angle);
//     TFloat t = 1 - c;

//     auto naxis = core::vnorm(axis);
//     TFloat x = naxis.x();
//     TFloat y = naxis.y();
//     TFloat z = naxis.z();

//     TFloat x2 = x * x;
//     TFloat y2 = y * y;
//     TFloat z2 = z * z;

//     TFloat xy = x * y;
//     TFloat xz = x * z;
//     TFloat yz = y * z;

//     TFloat xs = x * s;
//     TFloat ys = y * s;
//     TFloat zs = z * s;

//     TFloat m00 = t * x2 + c;
//     TFloat m01 = t * xy - zs;
//     TFloat m02 = t * xz + ys;
//     TFloat m10 = t * xy + zs;
//     TFloat m11 = t * y2 + c;
//     TFloat m12 = t * yz - xs;
//     TFloat m20 = t * xz - ys;
//     TFloat m21 = t * yz + xs;
//     TFloat m22 = t * z2 + c;

//     auto r = core::mat4x4<TFloat>(
//         m00, m01, m02, 0, // right
//         m10, m11, m12, 0, // up
//         m20, m21, m22, 0, // forward
//         0,   0,   0,   1
//     );

//     m = r * m;
// }


// template<typename TFloat>
// constexpr void rotate_right(vec2<TFloat>& v, const vec2<TFloat>& axis, TFloat angle) {
//     rotate(v, axis, -angle);
// }

// template<typename TFloat, i32 Dim>
// constexpr void rotate_right(arr<vec<Dim, TFloat>>& v, const vec<Dim, TFloat>& axis, TFloat angle) {
//     for (i32 i = 0; i < v.len(); ++i) {
//         rotate_right(v[i], axis, angle);
//     }
// }

// template<typename TFloat>
// constexpr void rotate_right(mat4x4<TFloat>& v, const vec3<TFloat>& axis, TFloat angle) {
//     rotate(v, axis, -angle);
// }

}
