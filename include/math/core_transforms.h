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

template<addr_size Dim, typename T> constexpr vec<Dim, T>  translate(const vec<Dim, T>& v, const vec<Dim, T>& t);
template<typename T>                constexpr mat3<T>      translate(const mat3<T>& m, const vec2<T>& t);
template<typename T>                constexpr mat4<T>      translate(const mat4<T>& m, const vec3<T>& t);

template<addr_size Dim, typename T> constexpr vec<Dim, T>  scale(const vec<Dim, T>& v, const vec<Dim, T>& s);
template<addr_size Dim, typename T> constexpr vec<Dim, T>  scale(const vec<Dim, T>& v, const vec<Dim, T>& pivot, const vec<Dim, T>& s);
template<typename T>                constexpr mat3<T>      scale(const mat3<T>& m, const vec2<T>& s);
template<typename T>                constexpr mat4<T>      scale(const mat4<T>& m, const vec3<T>& s);

template<typename TFloat>           constexpr vec2<TFloat> skewX(const vec2<TFloat>& v, TFloat tanAngle);
template<typename TFloat>           inline    vec2<TFloat> skewX(const vec2<TFloat>& v, core::radians angle);
template<typename TFloat>           constexpr mat3<TFloat> skewX(const mat3<TFloat>& m, TFloat tanAngle);
template<typename TFloat>           inline    mat3<TFloat> skewX(const mat3<TFloat>& m, core::radians angle);

template<typename TFloat>           constexpr vec2<TFloat> skewY(const vec2<TFloat>& v, TFloat tanAngle);
template<typename TFloat>           inline    vec2<TFloat> skewY(const vec2<TFloat>& v, core::radians angle);
template<typename TFloat>           constexpr mat3<TFloat> skewY(const mat3<TFloat>& m, TFloat tanAngle);
template<typename TFloat>           inline    mat3<TFloat> skewY(const mat3<TFloat>& m, core::radians angle);

// TODO: missing 3d skew implementation.

template<typename TFloat>           inline vec2<TFloat>   rotate(const vec2<TFloat>& v, const vec2<TFloat>& pivot, core::radians angle);
template<typename TFloat>           inline vec2<TFloat>   rotateRight(const vec2<TFloat>& v, const vec2<TFloat>& pivot, core::radians angle);
template<typename TFloat>           inline mat3<TFloat>   rotate(const mat3<TFloat>& m, const vec2<TFloat>& pivot, core::radians angle);
template<typename TFloat>           inline mat3<TFloat>   rotateRight(const mat3<TFloat>& m, const vec2<TFloat>& pivot, core::radians angle);

template<typename TFloat>           inline mat4<TFloat>   rotate(const mat4<TFloat>& m, const vec3<TFloat>& pivot, core::radians angle);
template<typename TFloat>           inline mat4<TFloat>   rotateRight(const mat4<TFloat>& v, const vec3<TFloat>& pivot, core::radians angle);
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
    // 2D homogeneous translation (memory order: m[col][row], column vectors):
    //
    // T(tx, ty) columns:
    // col0 = (1, 0, 0)
    // col1 = (0, 1, 0)
    // col2 = (tx, ty, 1)
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
    // 3D homogeneous translation (memory order: m[col][row], column vectors):
    //
    // T(tx, ty, tz) columns:
    // col0 = (1, 0, 0, 0)
    // col1 = (0, 1, 0, 0)
    // col2 = (0, 0, 1, 0)
    // col3 = (tx, ty, tz, 1)
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
    // v'_i = v_i * s_i
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
    // 2D homogeneous scaling (memory order: m[col][row], column vectors):
    //
    // S(sx, sy) columns:
    // col0 = (sx, 0, 0)
    // col1 = (0, sy, 0)
    // col2 = (0, 0, 1)
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
    // 3D homogeneous scaling (memory order: m[col][row], column vectors):
    //
    // S(sx, sy, sz) columns:
    // col0 = (sx, 0, 0, 0)
    // col1 = (0, sy, 0, 0)
    // col2 = (0, 0, sz, 0)
    // col3 = (0, 0, 0, 1)
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
    // 2D Cartesian skew along X:
    // x' = x + tanAngle * y
    // y' = y
    vec2<TFloat> ret = v;
    ret.x() = ret.x() + tanAngle * ret.y();
    return ret;
}


template<typename TFloat>
inline vec2<TFloat> skewX(const vec2<TFloat>& v, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // Angle overload:
    // tanAngle = tan(angle)
    // then x' = x + tanAngle * y, y' = y
    // Note: skew from angle uses tan(angle). Around 90deg + k*180deg tan grows unbounded;
    // call sites should clamp or guard near singularities (common threshold is |cos(angle)| <= 1e-6 for f32).
    return skewX(v, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr mat3<TFloat> skewX(const mat3<TFloat>& m, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    auto ret = m;
    // 2D homogeneous skew along X (memory order: m[col][row], column vectors):
    //
    // Sx(a) columns:
    // col0 = (1, 0, 0)
    // col1 = (a, 1, 0)
    // col2 = (0, 0, 1)
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
    // Angle overload:
    // a = tan(angle), then compose ret = m * Sx(a).
    return skewX(m, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr vec2<TFloat> skewY(const vec2<TFloat>& v, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // 2D Cartesian skew along Y:
    // x' = x
    // y' = y + tanAngle * x
    vec2<TFloat> ret = v;
    ret.y() = ret.y() + tanAngle * ret.x();
    return ret;
}

template<typename TFloat>
inline vec2<TFloat> skewY(const vec2<TFloat>& v, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // Angle overload:
    // tanAngle = tan(angle)
    // then x' = x, y' = y + tanAngle * x
    // Note: skew from angle uses tan(angle). Around 90deg + k*180deg tan grows unbounded;
    // call sites should clamp or guard near singularities (common threshold is |cos(angle)| <= 1e-6 for f32).
    return skewY(v, TFloat(core::tan(f32(angle))));
}

template<typename TFloat>
constexpr mat3<TFloat> skewY(const mat3<TFloat>& m, TFloat tanAngle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    auto ret = m;
    // 2D homogeneous skew along Y (memory order: m[col][row], column vectors):
    //
    // Sy(a) columns:
    // col0 = (1, a, 0)
    // col1 = (0, 1, 0)
    // col2 = (0, 0, 1)
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
    // Angle overload:
    // a = tan(angle), then compose ret = m * Sy(a).
    return skewY(m, TFloat(core::tan(f32(angle))));
}

//======================================================================================================================
// Rotate 2D
//======================================================================================================================

template<typename TFloat>
inline vec2<TFloat> rotate(const vec2<TFloat>& v, const vec2<TFloat>& origin, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");
    // 2D Cartesian rotation around pivot `origin`:
    //
    // v' = origin + R(a) * (v - origin)
    //
    // R(a) =
    // [  cos(a) -sin(a) ]
    // [  sin(a)  cos(a) ]
    //
    // Expanded:
    // x' = ox + (x - ox) * cos(a) - (y - oy) * sin(a)
    // y' = oy + (x - ox) * sin(a) + (y - oy) * cos(a)
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

template<typename TFloat>
inline mat3<TFloat> rotate(const mat3<TFloat>& m, const vec2<TFloat>& pivot, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));

    // 2D homogeneous rotation around pivot p=pivot (memory order: m[col][row], column vectors):
    //
    // R(a) columns:
    // col0 = ( c, s, 0)
    // col1 = (-s, c, 0)
    // col2 = ( 0, 0, 1)
    //
    // Composition uses post-multiply around pivot:
    // ret = m * T(p) * R(a) * T(-p)
    mat3<TFloat> r (
         c,  s, TFloat(0),
        -s,  c, TFloat(0),
         TFloat(0), TFloat(0), TFloat(1)
    );

    mat3<TFloat> ret = translate(m, pivot);
    ret = ret * r;
    ret = translate(ret, -pivot);
    return ret;
}

template<typename TFloat>
inline mat3<TFloat> rotateRight(const mat3<TFloat>& m, const vec2<TFloat>& pivot, core::radians angle) {
    angle.value = -angle.value;
    return rotate(m, pivot, angle);
}

//======================================================================================================================
// Rotate 3D
//======================================================================================================================

template<typename TFloat>
inline mat4<TFloat> rotate(const mat4<TFloat>& m, const vec3<TFloat>& pivot, core::radians angle) {
    static_assert(core::is_float_v<TFloat>, "type must be floating point");

    TFloat c = core::cos(f32(angle));
    TFloat s = core::sin(f32(angle));
    TFloat t = TFloat(1) - c;

    // 3D homogeneous rotation around pivot (memory order: m[col][row], column vectors):
    //
    // Let a = normalize(pivot), a = (ax, ay, az), and t = 1 - cos(angle).
    // R(angle, a) columns:
    // col0 = (t*ax*ax + c,     t*ax*ay - s*az,  t*ax*az + s*ay, 0)
    // col1 = (t*ax*ay + s*az,  t*ay*ay + c,     t*ay*az - s*ax, 0)
    // col2 = (t*ax*az - s*ay,  t*ay*az + s*ax,  t*az*az + c,    0)
    // col3 = (0, 0, 0, 1)
    //
    // Composition uses post-multiply:
    // ret = m * R(angle, a)

#if CORE_DEBUG
    // Degenerate axis: rotation is undefined, treat as no-op.
    f64 pivotLenSq = core::vlengthsq(pivot);
    if (core::safeEq(pivotLenSq, 0.0, 1e-12)) {
        AssertFmt(false, "Degenerate pivot axis {:f.10}", pivotLenSq);
    }
#endif

    vec3<TFloat> a = pivot.norm(); // I could assume that the pivot is always normalized to avoid this call.

    auto m00 = t * a.x() * a.x() + c;
    auto m10 = t * a.x() * a.y() - s * a.z();
    auto m20 = t * a.x() * a.z() + s * a.y();
    auto m01 = t * a.x() * a.y() + s * a.z();
    auto m11 = t * a.y() * a.y() + c;
    auto m21 = t * a.y() * a.z() - s * a.x();
    auto m02 = t * a.x() * a.z() - s * a.y();
    auto m12 = t * a.y() * a.z() + s * a.x();
    auto m22 = t * a.z() * a.z() + c;

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
inline mat4<TFloat> rotateRight(const mat4<TFloat>& v, const vec3<TFloat>& pivot, core::radians angle) {
    angle.value = -angle.value;
    return rotate(v, pivot, angle);
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
