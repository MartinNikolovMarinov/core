#pragma once

#include <core_arr.h>
#include <core_assert_fmt.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>
#include <math/core_matrix.h>
#include <math/core_vec.h>

namespace core {

using namespace coretypes;

//======================================================================================================================
// Forward Declarations
//======================================================================================================================

template<addr_size Dim, typename T> constexpr vec<Dim, T> translate(const vec<Dim, T>& v, const vec<Dim, T>& t);
template<typename T>                constexpr mat3<T>     translate(const mat3<T>& m, const vec2<T>& t);
template<typename T>                constexpr mat4<T>     translate(const mat4<T>& m, const vec3<T>& t);

template<addr_size Dim, typename T> constexpr vec<Dim, T> scale(const vec<Dim, T>& v, const vec<Dim, T>& s);
template<addr_size Dim, typename T> constexpr vec<Dim, T> scale(const vec<Dim, T>& v, const vec<Dim, T>& pivot, const vec<Dim, T>& s);
template<typename T>                constexpr mat3<T>     scale(const mat3<T>& m, const vec2<T>& s);
template<typename T>                constexpr mat4<T>     scale(const mat4<T>& m, const vec3<T>& s);

template<typename TFloat>           constexpr vec2<TFloat> skewX(const vec2<TFloat>& v, TFloat tanAngle);
template<typename TFloat>           inline    vec2<TFloat> skewX(const vec2<TFloat>& v, core::radians angle);
template<typename TFloat>           constexpr mat3<TFloat> skewX(const mat3<TFloat>& m, TFloat tanAngle);
template<typename TFloat>           inline    mat3<TFloat> skewX(const mat3<TFloat>& m, core::radians angle);

template<typename TFloat>           constexpr vec2<TFloat> skewY(const vec2<TFloat>& v, TFloat tanAngle);
template<typename TFloat>           inline    vec2<TFloat> skewY(const vec2<TFloat>& v, core::radians angle);
template<typename TFloat>           constexpr mat3<TFloat> skewY(const mat3<TFloat>& m, TFloat tanAngle);
template<typename TFloat>           inline    mat3<TFloat> skewY(const mat3<TFloat>& m, core::radians angle);

template<addr_size Dim, typename T> constexpr vec<Dim, T> sheer(const vec<Dim, T>& v, const vec<Dim, T>& s);

template<typename TFloat>           inline vec2<TFloat>   rotate(const vec2<TFloat>& v, const vec2<TFloat>& origin, core::radians angle);
template<typename TFloat>           inline vec2<TFloat>   rotateRight(const vec2<TFloat>& v, const vec2<TFloat>& origin, core::radians angle);

template<typename TFloat>           inline mat4<TFloat>   rotate(const mat4<TFloat>& m, const vec3<TFloat>& a, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateRight(const mat4<TFloat>& v, const vec3<TFloat>& axis, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateX(const mat4<TFloat>& m, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateXRight(const mat4<TFloat>& m, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateY(const mat4<TFloat>& m, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateYRight(const mat4<TFloat>& m, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateZ(const mat4<TFloat>& m, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateZRight(const mat4<TFloat>& m, core::radians angle);

//======================================================================================================================
// Translate
//======================================================================================================================

template<addr_size Dim, typename T>
constexpr vec<Dim, T> translate(const vec<Dim, T>& v, const vec<Dim, T>& t) {
    // Vector translation in Cartesian space:
    //
    // v' = v + t
    //
    // Expanded per component:
    // v'_i = v_i + t_i
    vec<Dim, T> ret = {};
    for (addr_size i = 0; i < v.dimensions(); ++i) {
        ret[i] = v[i] + t[i];
    }
    return ret;
}

template<typename T>
constexpr mat3<T> translate(const mat3<T>& m, const vec2<T>& t) {
    mat3<T> ret = m;
    // 2D homogeneous translation (column-major, column vectors):
    //
    // T(tx, ty) =
    // [ 1  0  0 ]
    // [ 0  1  0 ]
    // [ tx ty 1 ]
    //
    // Composition uses post-multiply: ret = m * T.
    // In this storage layout translation lives in column 2.
    // Expanded column update:
    // ret.col2 = m.col0 * tx + m.col1 * ty + m.col2
    ret[2][0] = m[0][0] * t.x() + m[1][0] * t.y() + m[2][0];
    ret[2][1] = m[0][1] * t.x() + m[1][1] * t.y() + m[2][1];
    ret[2][2] = m[0][2] * t.x() + m[1][2] * t.y() + m[2][2];
    return ret;
}

template<typename T>
constexpr mat4<T> translate(const mat4<T>& m, const vec3<T>& t) {
    mat4<T> ret = m;
    // 3D homogeneous translation (column-major, column vectors):
    //
    // T(tx, ty, tz) =
    // [ 1   0   0   0 ]
    // [ 0   1   0   0 ]
    // [ 0   0   1   0 ]
    // [ tx  ty  tz  1 ]
    //
    // Composition uses post-multiply: ret = m * T.
    // In this storage layout translation lives in column 3.
    // Expanded column update:
    // ret.col3 = m.col0 * tx + m.col1 * ty + m.col2 * tz + m.col3
    ret[3][0] = m[0][0] * t.x() + m[1][0] * t.y() + m[2][0] * t.z() + m[3][0];
    ret[3][1] = m[0][1] * t.x() + m[1][1] * t.y() + m[2][1] * t.z() + m[3][1];
    ret[3][2] = m[0][2] * t.x() + m[1][2] * t.y() + m[2][2] * t.z() + m[3][2];
    ret[3][3] = m[0][3] * t.x() + m[1][3] * t.y() + m[2][3] * t.z() + m[3][3];
    return ret;
}

//======================================================================================================================
// Scale
//======================================================================================================================

template<addr_size Dim, typename T>
constexpr vec<Dim, T> scale(const vec<Dim, T>& v, const vec<Dim, T>& s) {
    // Vector scaling in Cartesian space:
    //
    // v' = v * s
    //
    // Expanded per component:
    // v'_i = v_i * s
    auto ret = v * s;
    return ret;
}

template<addr_size Dim, typename T>
constexpr vec<Dim, T> scale(const vec<Dim, T>& v, const vec<Dim, T>& pivot, const vec<Dim, T>& s) {
    // Vector scaling around an arbitrary pivot (anchor) in Cartesian space:
    //
    // v' = pivot + (v - pivot) * s
    //
    // Interpretation:
    // 1) move v into pivot-relative coordinates: (v - pivot)
    // 2) scale component-wise by s
    // 3) move back by adding pivot
    //
    // Expanded per component:
    // v'_i = pivot_i + (v_i - pivot_i) * s_i
    //
    // Useful for "scale around point P" behavior:
    // - s = 1 keeps that axis unchanged
    // - s = 0 collapses that axis to pivot on that component
    // - negative factors mirror around pivot on that component
    vec<Dim, T> ret = {};
    for (addr_size i = 0; i < v.dimensions(); ++i) {
        ret[i] = pivot[i] + (v[i] - pivot[i]) * s[i];
    }
    return ret;
}

template<typename T> constexpr mat3<T> scale(const mat3<T>& m, const vec2<T>& s) {
    auto ret = m;
    // 2D homogeneous scaling (column-major, column vectors):
    //
    // S(sx, sy) =
    // [ sx  0   0 ]
    // [ 0   sy  0 ]
    // [ 0   0   1 ]
    //
    // Composition uses post-multiply: ret = m * S.
    // In this storage layout, post-multiplying by S scales whole columns:
    // ret.col0 = m.col0 * sx
    // ret.col1 = m.col1 * sy
    // ret.col2 = m.col2
    ret[0][0] *= s.x(); ret[0][1] *= s.x(); ret[0][2] *= s.x();
    ret[1][0] *= s.y(); ret[1][1] *= s.y(); ret[1][2] *= s.y();
    return ret;
}

template<typename T>
constexpr mat4<T> scale(const mat4<T>& m, const vec3<T>& s) {
    auto ret = m;
    // 3D homogeneous scaling (column-major, column vectors):
    //
    // S(sx, sy, sz) =
    // [ sx  0   0   0 ]
    // [ 0   sy  0   0 ]
    // [ 0   0   sz  0 ]
    // [ 0   0   0   1 ]
    //
    // Composition uses post-multiply: ret = m * S.
    // In this storage layout, post-multiplying by S scales whole columns:
    // ret.col0 = m.col0 * sx
    // ret.col1 = m.col1 * sy
    // ret.col2 = m.col2 * sz
    // ret.col3 = m.col3
    ret[0][0] *= s.x(); ret[0][1] *= s.x(); ret[0][2] *= s.x(); ret[0][3] *= s.x();
    ret[1][0] *= s.y(); ret[1][1] *= s.y(); ret[1][2] *= s.y(); ret[1][3] *= s.y();
    ret[2][0] *= s.z(); ret[2][1] *= s.z(); ret[2][2] *= s.z(); ret[2][3] *= s.z();
    return ret;
}

//======================================================================================================================
// Sheer/Skew
//======================================================================================================================

template<typename TFloat>
constexpr vec2<TFloat> skewX(const vec2<TFloat>& v, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    vec2<TFloat> ret = v;
    ret.x() = ret.x() + tanAngle * ret.y();
    return ret;
}


template<typename TFloat>
inline vec2<TFloat> skewX(const vec2<TFloat>& v, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // Note: skew from angle uses tan(angle). Around 90deg + k*180deg tan grows unbounded;
    // call sites should clamp or guard near singularities (common threshold is |cos(angle)| <= 1e-6 for f32).
    return skewX(v, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr mat3<TFloat> skewX(const mat3<TFloat>& m, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    auto ret = m;
    // 2D homogeneous skew along X (column-major, column vectors):
    //
    // Sx(a) =
    // [ 1  0  0 ]
    // [ a  1  0 ]
    // [ 0  0  1 ]
    //
    // where a = tanAngle.
    //
    // Composition uses post-multiply: ret = m * Sx(a).
    // In this storage layout, post-multiplying by Sx updates column 1:
    //
    // ret.col0 = m.col0
    // ret.col1 = m.col0 * a + m.col1
    // ret.col2 = m.col2
    ret[1][0] += m[0][0] * tanAngle;
    ret[1][1] += m[0][1] * tanAngle;
    ret[1][2] += m[0][2] * tanAngle;
    return ret;
}

template<typename TFloat>
inline mat3<TFloat> skewX(const mat3<TFloat>& m, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    return skewX(m, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr vec2<TFloat> skewY(const vec2<TFloat>& v, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    vec2<TFloat> ret = v;
    ret.y() = ret.y() + tanAngle * ret.x();
    return ret;
}

template<typename TFloat>
inline vec2<TFloat> skewY(const vec2<TFloat>& v, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // Note: skew from angle uses tan(angle). Around 90deg + k*180deg tan grows unbounded;
    // call sites should clamp or guard near singularities (common threshold is |cos(angle)| <= 1e-6 for f32).
    return skewY(v, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr mat3<TFloat> skewY(const mat3<TFloat>& m, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    auto ret = m;
    // 2D homogeneous skew along Y (column-major, column vectors):
    //
    // Sy(a) =
    // [ 1  a  0 ]
    // [ 0  1  0 ]
    // [ 0  0  1 ]
    //
    // where a = tanAngle.
    //
    // Composition uses post-multiply: ret = m * Sy(a).
    // In this storage layout, post-multiplying by Sy updates column 0:
    //
    // ret.col0 = m.col0 + m.col1 * a
    // ret.col1 = m.col1
    // ret.col2 = m.col2
    ret[0][0] += m[1][0] * tanAngle;
    ret[0][1] += m[1][1] * tanAngle;
    ret[0][2] += m[1][2] * tanAngle;
    return ret;
}

template<typename TFloat>
inline mat3<TFloat> skewY(const mat3<TFloat>& m, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    return skewY(m, TFloat(core::tan(f32(angle))));
}

//======================================================================================================================
// Rotate 2D
//======================================================================================================================

template<typename TFloat>
inline vec2<TFloat> rotate(const vec2<TFloat>& v, const vec2<TFloat>& origin, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    vec2<TFloat> ret = v;
    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));
    ret -= origin;
    ret = core::v<TFloat>(ret.x() * c - ret.y() * s, ret.x() * s + ret.y() * c);
    ret += origin;
    return ret;
}

template<typename TFloat>
inline vec2<TFloat> rotateRight(const vec2<TFloat>& v, const vec2<TFloat>& origin, core::radians angle) {
    angle.value = -angle.value;
    return rotate(v, origin, angle);
}

//======================================================================================================================
// Rotate 3D
//======================================================================================================================

template<typename TFloat>
inline mat4<TFloat> rotate(const mat4<TFloat>& m, const vec3<TFloat>& a, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));
    TFloat t = TFloat(1) - c;

#if CORE_DEBUG
    // Degenerate axis: rotation is undefined, treat as no-op.
    f64 axisLenSq = core::vlengthsq(a);
    if (core::safeEq(axisLenSq, 0.0, 1e-12)) {
        AssertFmt(false, "Degenerate axis {:f.10}", axisLenSq);
    }
#endif

    vec3<TFloat> axis = a.norm(); // I could assume that the axis is always normalized to avoid this call.

    auto m00 = t * axis.x() * axis.x() + c;
    auto m10 = t * axis.x() * axis.y() - s * axis.z();
    auto m20 = t * axis.x() * axis.z() + s * axis.y();
    auto m01 = t * axis.x() * axis.y() + s * axis.z();
    auto m11 = t * axis.y() * axis.y() + c;
    auto m21 = t * axis.y() * axis.z() - s * axis.x();
    auto m02 = t * axis.x() * axis.z() - s * axis.y();
    auto m12 = t * axis.y() * axis.z() + s * axis.x();
    auto m22 = t * axis.z() * axis.z() + c;

    mat4<TFloat> r (
        m00, m10, m20, 0,
        m01, m11, m21, 0,
        m02, m12, m22, 0,
          0,   0,   0, 1
    );

    auto ret = m * r;
    return ret;
}

template<typename TFloat>
inline mat4<TFloat> rotateRight(const mat4<TFloat>& v, const vec3<TFloat>& axis, core::radians angle) {
    angle.value = -angle.value;
    return rotate(v, axis, angle);
}

template<typename TFloat>
inline mat4<TFloat> rotateX(const mat4<TFloat>& m, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));

    mat4<TFloat> r (
        1, 0, 0, 0,
        0, c, s, 0,
        0,-s, c, 0,
        0, 0, 0, 1
    );

    auto ret = m * r;
    return ret;
}

template<typename TFloat>
inline mat4<TFloat> rotateXRight(const mat4<TFloat>& m, core::radians angle) {
    angle.value = -angle.value;
    return rotateX(m, angle);
}

template<typename TFloat>
inline mat4<TFloat> rotateY(const mat4<TFloat>& m, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));

    mat4<TFloat> r (
        c, 0,-s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
    );

    auto ret = m * r;
    return ret;
}

template<typename TFloat>
inline mat4<TFloat> rotateYRight(const mat4<TFloat>& m, core::radians angle) {
    angle.value = -angle.value;
    return rotateY(m, angle);
}

template<typename TFloat>
inline mat4<TFloat> rotateZ(const mat4<TFloat>& m, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));

    mat4<TFloat> r (
        c, s, 0, 0,
       -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    auto ret = m * r;
    return ret;
}

template<typename TFloat>
inline mat4<TFloat> rotateZRight(const mat4<TFloat>& m, core::radians angle) {
    angle.value = -angle.value;
    return rotateZ(m, angle);
}

}
