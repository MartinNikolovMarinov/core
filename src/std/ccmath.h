#pragma once

#include <cmath>

#include <types.h>
#include <core_math.h>
#include <intrinsic.h>

namespace core {

#pragma region Classification -----------------------------------------------------------------------------------------

constexpr f32 infinity_f32()      { return i_huge_valf(); }
constexpr f32 quiet_NaN_f32()     { return i_nanf(); }
constexpr f32 signaling_NaN_f32() { return i_nansf(); }

constexpr f64 infinity_f64()      { return i_huge_val(); }
constexpr f64 quiet_NaN_f64()     { return i_nan(); }
constexpr f64 signaling_NaN_f64() { return i_nans(); }

namespace detail {

template <typename TFloat>
constexpr bool isinf_compiletime_impl(TFloat x) {
    return x == infinity_f32() || -x == infinity_f32();
}

template <typename TFloat>
constexpr bool isinf(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return detail::isinf_compiletime_impl(x);
    }
    return std::isinf(x);
}

} // namespace detail

constexpr bool isinf(f32 x) { return detail::isinf(x); }
constexpr bool isinf(f64 x) { return detail::isinf(x); }

namespace detail {

template <typename TFloat>
constexpr bool isnan_compiletime_impl(TFloat x) {
    return x != x;
}

template <typename TFloat>
constexpr bool isnan(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return detail::isnan_compiletime_impl(x);
    }
    return std::isnan(x);
}

} // namespace detail

constexpr bool isnan(f32 x) { return detail::isnan(x); }
constexpr bool isnan(f64 x) { return detail::isnan(x); }

namespace detail {

template <typename TFloat>
constexpr bool isnormal_compiletime_impl(TFloat x) {
    bool res = x != 0 &&
               !detail::isinf_compiletime_impl(x) &&
               !detail::isnan_compiletime_impl(x) &&
                core::abs(x) >= core::limit_min<TFloat>();
    return res;
}

template <typename TFloat>
constexpr bool isnormal(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return detail::isnormal_compiletime_impl(x);
    }
    return std::isnormal(x);
}

} // namespace detail

constexpr bool isnormal(f32 x) { return detail::isnormal(x); }
constexpr bool isnormal(f64 x) { return detail::isnormal(x); }

namespace detail {

template <typename TFloat>
constexpr i32 fpclassify_compiletime_impl(TFloat x) {
    if (detail::isnan_compiletime_impl(x))                            return FP_NAN;
    if (detail::isinf_compiletime_impl(x))                            return FP_INFINITE;
    if (core::abs(x) == TFloat(0))                                    return FP_ZERO;
    if (core::abs(x) > 0 && core::abs(x) < core::limit_min<TFloat>()) return FP_SUBNORMAL;
    return FP_NORMAL;
}

template <typename TFloat>
constexpr i32 fpclassify(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return fpclassify_compiletime_impl(x);
    }
    return std::fpclassify(x);
}

} // namespace detail

constexpr i32 fpclassify(f32 x) { return detail::fpclassify(x); }
constexpr i32 fpclassify(f64 x) { return detail::fpclassify(x); }

#pragma endregion


#pragma region Floor -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat floor_compiletime_impl(TFloat x) {
    if (core::abs(x) == TFloat(0)) return x;
    if (core::isinf(x))            return x;
    if (core::isnan(x))            return x;

    if (x >= 0) {
        return (x < 1) ? TFloat(0) : TFloat(u64(x));
    }

    if (x > -1) return TFloat(-1);
    u64 posIntPart = u64(-x);
    if (TFloat(posIntPart) == -x) return x; // no fractional part.
    return -TFloat(posIntPart + 1);
}

template <typename TFloat>
constexpr TFloat floor(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return floor_compiletime_impl(x);
    }
    return std::floor(x);
}

} // detail

constexpr f32 floor(f32 x) { return detail::floor(x); }
constexpr f64 floor(f64 x) { return detail::floor(x); }

#pragma endregion

#pragma region Ceil --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat ceil_compiletime_impl(TFloat x) {
    if (core::abs(x) == TFloat(0)) return x;
    if (core::isinf(x))            return x;
    if (core::isnan(x))            return x;

    TFloat res = core::detail::floor_compiletime_impl(x);
    return res == x ? res : res + TFloat(1);
}

template <typename TFloat>
constexpr TFloat ceil(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return ceil_compiletime_impl(x);
    }
    return std::ceil(x);
}

} // namespace detail

constexpr f32 ceil(f32 x) { return detail::ceil(x); }
constexpr f64 ceil(f64 x) { return detail::ceil(x); }

#pragma endregion

#pragma region Trunc -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat trunc_compiletime_impl(TFloat x) {
    if (core::abs(x) == TFloat(0)) return x;
    if (core::isinf(x))            return x;
    if (core::isnan(x))            return x;

    return (x > 0) ? floor_compiletime_impl(x) : ceil_compiletime_impl(x);
}

template <typename TFloat>
constexpr TFloat trunc(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return trunc_compiletime_impl(x);
    }
    return std::trunc(x);
}

} // namespace detail

constexpr f32 trunc(f32 x) { return detail::trunc(x); }
constexpr f64 trunc(f64 x) { return detail::trunc(x); }

#pragma endregion

#pragma region Modf --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat modf_error_impl(TFloat x, TFloat* iptr) {
    *iptr = x;
    if (core::abs(x) == TFloat(0)) return x;
    return x > TFloat(0) ? TFloat(0) : -TFloat(0);
}

template <typename TFloat>
constexpr TFloat modf_nan_impl(TFloat x, TFloat* iptr) {
    *iptr = x;
    return x;
}

template <typename TFloat>
constexpr TFloat modf_compiletime_impl(TFloat x, TFloat* iptr) {
    if (detail::isnan_compiletime_impl(x)) return modf_nan_impl(x, iptr);
    if (detail::isinf_compiletime_impl(x)) return modf_error_impl(x, iptr);
    if (core::abs(x) == TFloat(0))         return modf_error_impl(x, iptr);

    *iptr = core::detail::trunc_compiletime_impl(x);
    return (x - *iptr);
}

template <typename TFloat>
constexpr TFloat modf(TFloat x, TFloat* iptr) {
    if constexpr (core::is_const_evaluated()) {
        return modf_compiletime_impl(x, iptr);
    }
    return std::modf(x, iptr);
}

} // namespace detail

constexpr f32 modf(f32 x, f32* iptr) { return detail::modf(x, iptr); }
constexpr f64 modf(f64 x, f64* iptr) { return detail::modf(x, iptr); }

#pragma endregion

#pragma region Round -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat round_compiletime_impl(TFloat x) {
    if (detail::isnan_compiletime_impl(x)) return x;
    if (detail::isinf_compiletime_impl(x)) return x;
    if (core::abs(x) == TFloat(0))         return x;

    TFloat iptr = 0;
    const TFloat modx = core::detail::modf_compiletime_impl(x, &iptr);
    constexpr TFloat half = TFloat(1)/2;

    if (modx >= half && iptr >= 0) return iptr + 1;
    if (core::abs(modx) >= half && iptr <= 0) return iptr - 1;
    return iptr;
}

template <typename TFloat>
constexpr TFloat round(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return round_compiletime_impl(x);
    }
    return std::round(x);
}

} // namespace detail

constexpr f32 round(f32 x) { return detail::round(x); }
constexpr f64 round(f64 x) { return detail::round(x); }

template <typename TFloat>
constexpr TFloat round_to(TFloat n, u32 to) {
    static_assert(core::is_float_v<TFloat>, "Invalid TFloat argument.");
    return TFloat(round(n * TFloat(10*to))) / TFloat(10*to);
}

#pragma endregion

#pragma region Sqrt --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat sqrt_compiletime_impl(TFloat x) {
    // Newton's algorithm
    if (x < TFloat(0))  return core::quiet_NaN_f32();
    if (x == TFloat(0)) return TFloat(0);
    TFloat z = TFloat(1);
    constexpr i32 itter = 10;
    for (i32 i = 1; i <= itter; i++) {
        z -= (z*z - x) / (2*z);
    }
    return z;
}

template <typename TFloat>
constexpr TFloat sqrt(TFloat x) {
    if constexpr (core::is_const_evaluated()) {
        return sqrt_compiletime_impl(x);
    }
    return std::sqrt(x);
}

} // namespace detail


constexpr f32 sqrt(f32 x) { return detail::sqrt(x); }
constexpr f64 sqrt(f64 x) { return detail::sqrt(x); }

#pragma endregion

#pragma region Pow --------------------------------------------------------------------------------------------------

// TODO: A cosntexpr version of pow is possible, but it will significant differences with std::pow on different standard
//       libraries, unless it's implemented very carefully. That sounds annoying to do, so I'll leave it for now.

inline f32 pow(f32 x, f32 exp) { return ::powf(x, exp); }
inline f64 pow(f64 x, f32 exp) { return ::pow(x, exp); }

#pragma endregion

#pragma region Trig --------------------------------------------------------------------------------------------------

inline f32 sin(f32 x) { return std::sin(x); }
inline f64 sin(f64 x) { return std::sin(x); }

inline f32 cos(f32 x) { return std::cos(x); }
inline f64 cos(f64 x) { return std::cos(x); }

inline f32 tan(f32 x) { return std::tan(x); }
inline f64 tan(f64 x) { return std::tan(x); }

inline f32 asin(f32 x) { return std::asin(x); }
inline f64 asin(f64 x) { return std::asin(x); }

inline f32 acos(f32 x) { return std::acos(x); }
inline f64 acos(f64 x) { return std::acos(x); }

inline f32 atan(f32 x) { return std::atan(x); }
inline f64 atan(f64 x) { return std::atan(x); }

inline f32 atan2(f32 a, f32 b) { return std::atan2(a, b); }
inline f64 atan2(f64 a, f64 b) { return std::atan2(a, b); }

#pragma endregion

} // namespace core
