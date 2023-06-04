#pragma once

#include <API.h>
#include <types.h>
#include <tuple.h>

// IMPORTANT: Implementation of standard functions uses musl libc as a reference.
//
// Here is a representation of a floating point number in memory (Little Endian) as a reference for some of the bitwise
// operations:
//
//   31 bit 30 bit     23 bit                    0 bit
//   |      |          |                         |
//   | Sign | Exponent |        Mantissa         |
//   |  1   | 10000010 | 10001100000000000000000 |
//
// This number in decimal is -12.375.

// TODO: Some of the functions in this file can be implemented MUCH faster for specific hardware with assembly.
//       At some point I should invest the time to do this.
//       Until then write good test so that the transition can be less painful.

// TODO: I should probably define some INFINITY and NAN constants for f32 and f64. It's not as easy as it sounds though.

namespace core {

using namespace coretypes;

constexpr u64 pow_u64(i64 n, u32 p) {
    if (n == 0) return 0;
    if (p == 0) return 1;
    u64 result = 1;
    while(p > 0) {
        if((p & 1) != 0) result = (result * n);
        n = (n * n);
        p = p >> 1; // p^2
    }
    return result;
}

#pragma region Radians/Degrees ----------------------------------------------------------------------------------------

struct radians {
    f32 value;
    radians() = default;
    radians(const radians&) = default;
    radians(radians&&) = default;
    constexpr explicit radians(f32 v) : value(v) {}
    constexpr operator f32() const { return value; }
};

// Should be obvious to the compiler that this struct is a very simple wrapper over a single f32.
static_assert(core::IsTrivial_v<radians>);
static_assert(sizeof(radians) == sizeof(f32));

constexpr radians deg_to_rad(f32 n) {
    return radians(n * (PI / 180.0f));
}

constexpr f32 rad_to_deg(const radians& n) {
    return n.value * (180.0f / PI);
}

#pragma endregion

#pragma region Take Exponent/Mantisssa --------------------------------------------------------------------------------

inline constexpr i32 exponent(f32 n) {
    union { f32 f; i32 i; } u = { n };
    i32 ret = (i32)(u.i >> 23 & 0xff);
    return ret;
}

inline constexpr i32 exponent(f64 n) {
    union { f64 f; i64 i; } u = { n };
    i32 ret = (i32)(u.i >> 52 & 0x7ff);
    return ret;
}

inline constexpr i32 mantissa(f32 n) {
    union { f32 f; i32 i; } u = { n };
    i32 ret = (i32)(u.i & 0x7fffff);
    return ret;
}

inline constexpr i64 mantissa(f64 n) {
    union { f64 f; i64 i; } u = { n };
    i64 ret = (i64)(u.i & 0xfffffffffffff);
    return ret;
}

#pragma endregion

#pragma region Classification -----------------------------------------------------------------------------------------

// The FP classification algorithm is from the musl libc implementaton.

enum struct FP_Classification {
    NAN = 0,
    INFINITE = 1,
    ZERO = 2,
    SUBNORMAL = 3,
    NORMAL = 4
};

constexpr FP_Classification fpclassify(f32 x) {
    union { f32 f; u32 i; } u = { x };
    i32 e = (u.i >> 23) & 0xff;
    if (!e) return u.i << 1 ? FP_Classification::SUBNORMAL : FP_Classification::ZERO;
    if (e == 0xff) return u.i << 9 ? FP_Classification::NAN : FP_Classification::INFINITE;
    return FP_Classification::NORMAL;
}

constexpr FP_Classification fpclassify(f64 x) {
    union { f64 f; u64 i; } u = { x };
    i32 e = (u.i >> 52) & 0x7ff;
    if (!e) return u.i << 1 ? FP_Classification::SUBNORMAL : FP_Classification::ZERO;
    if (e == 0x7ff) return u.i << 12 ? FP_Classification::NAN : FP_Classification::INFINITE;
    return FP_Classification::NORMAL;
}

template <typename TFloat>
constexpr bool isinf(TFloat x) {
    return fpclassify(x) == FP_Classification::INFINITE;
}

template <typename TFloat>
constexpr bool isnan(TFloat x) {
    return fpclassify(x) == FP_Classification::NAN;
}

template <typename TFloat>
constexpr bool isnormal(TFloat x) {
    return fpclassify(x) == FP_Classification::NORMAL;
}

#pragma endregion

#pragma region Sign --------------------------------------------------------------------------------------------------

constexpr i32 sign(f32 n) {
    union { f32 f; u32 i; } u = { n };
    return u.i >> 31;
}

constexpr i32 sign(f64 n) {
    union { f64 f; u64 i; } u = { n };
    return u.i >> 63;
}

#pragma endregion

#pragma region Round -------------------------------------------------------------------------------------------------

static constexpr f32 __f32toInt = 1 / core::EPSILON_F32;
static constexpr f64 __f64toInt = 1 / core::EPSILON_F64;

constexpr f32 round(f32 x) {
    union {f32 f; u32 i;} u = {x};
    i32 e = exponent(x);

    if (e >= 0x7f+23) {
        // The 0x7f value represents the exponent bias of the single-precision floating-point format. If the exponent is
        // greater than or equal to 0x7f+23, it means that the number x is either NaN (Not a Number) or infinite. In
        // such cases, the original value of x is returned as there is no need for rounding.
        return x;
    }

    // This condition checks if the sign bit of the number x is set (the most significant bit). If the sign bit is set,
    // it means that x is negative. In that case, the code negates the value of x.
    if (u.i >> 31) x = -x;

    if (e < 0x7f-1) {
        // If the exponent is less than 0x7f-1, it means that the absolute value of x is less than 1, in which case the
        // code returns 0.
        return 0*u.f;
    }

    // Lastly we actually round the number:
    f32 y = x + __f32toInt - __f32toInt - x;
    if (y > 0.5f)        y = y + x - 1;
    else if (y <= -0.5f) y = y + x + 1;
    else                 y = y + x;

    // If the original number was negative we need to negate the result:
    if (u.i >> 31) y = -y;
    return y;
}

// Musl implementation of round.
constexpr f64 round(f64 x) {
    union {f64 f; u64 i;} u = {x};
    i32 e = exponent(x);

    if (e >= 0x3ff+52) {
        // The 0x3ff value represents the exponent bias of the double-precision floating-point format. If the exponent
        // is greater than or equal to 0x3ff+52, it means that the number x is either NaN (Not a Number) or infinite.
        // In such cases, the original value of x is returned as there is no need for rounding.
        return x;
    }

    // This condition checks if the sign bit of the number x is set (the most significant bit). If the sign bit is set,
    // it means that x is negative. In that case, the code negates the value of x.
    if (u.i >> 63) x = -x;

    if (e < 0x3ff-1) {
        // If the exponent is less than 0x3ff-1, it means that the absolute value of x is less than 1, in which case
        // the code returns 0.
        return 0*u.f;
    }

    // Lastly we actually round the number:
    f64 y = x + __f64toInt - __f64toInt - x;
    if (y > 0.5)         y = y + x - 1;
    else if (y <= -0.5)  y = y + x + 1;
    else                 y = y + x;

    // If the original number was negative we need to negate the result:
    if (u.i >> 63) y = -y;
    return y;
}

template <typename TFloat>
constexpr TFloat round_to(TFloat n, u32 to) {
    return TFloat(round(n * TFloat(10*to))) / TFloat(10*to);
}

#pragma endregion

#pragma region Floor -------------------------------------------------------------------------------------------------

constexpr f32 floor(f32 x) {
    union { f32 f; u32 i; } u = {x};
    // Subtracting 0x7f compensates for the exponent bias in the IEEE 754 double-precision floating-point format.
    i32 e = exponent(x) - 0x7f;

    if (e >= 23) {
        // This means that the input x is already a whole number or has a magnitude greater than or equal to 1.
        return x;
    }

    if (e >= 0) {
        // The fractional part is within the significand of x.
        u32 m = 0x007fffff >> e;
        if ((u.i & m) == 0) return x; // fractional part is zero
        if (u.i >> 31) u.i += m; // the actual flooring
        u.i &= ~m;
    } else {
        if (u.i >> 31 == 0) u.i = 0;
        else if (u.i << 1) u.f = -1.0; // the actual flooring
    }

    return u.f;
}

constexpr f64 floor(f64 x) {
    union { f64 f; u64 i; } u = {x};
    // Subtracting 0x3ff compensates for the exponent bias in the IEEE 754 double-precision floating-point format.
    i32 e = exponent(x) - 0x3ff;

    if (e >= 52) {
        // This means that the input x is already a whole number or has a magnitude greater than or equal to 1.
        return x;
    }

    if (e >= 0) {
        // The fractional part is within the significand of x.
        u64 m = 0x000fffffffffffff >> e;
        if ((u.i & m) == 0) return x; // fractional part is zero
        if (u.i >> 63) u.i += m; // the actual flooring
        u.i &= ~m;
    } else {
        if (u.i >> 63 == 0) u.i = 0;
        else if (u.i << 1) u.f = -1.0; // the actual flooring
    }

    return u.f;
}

#pragma endregion

#pragma region Ceil --------------------------------------------------------------------------------------------------

constexpr f32 ceil(f32 x) {
    union { f32 f; u32 i; } u = { x };
    // Subtracting 0x7f compensates for the exponent bias in the IEEE 754 double-precision floating-point format.
    i32 e = exponent(x) - 0x7f;

    if (e >= 23) {
        // This means that the input x is already a whole number or has a magnitude greater than or equal to 1.
        return x;
    }

    if (e >= 0) {
        u32 m = 0x007fffff >> e;
        if ((u.i & m) == 0) return x; // fractional part is zero
        if (u.i >> 31 == 0) u.i += m; // the actual rounding
        u.i &= ~m;
    } else {
        if (u.i >> 31) u.f = -0.0;
        else if (u.i << 1) u.f = 1.0; // the actual rounding
    }

    return u.f;
}

constexpr f64 ceil(f64 x) {
    union { f64 f; u64 i; } u = { x };
    // Subtracting 0x3ff compensates for the exponent bias in the IEEE 754 double-precision floating-point format.
    i32 e = exponent(x) - 0x3ff;

    if (e >= 52) {
        // This means that the input x is already a whole number or has a magnitude greater than or equal to 1.
        return x;
    }

    if (e >= 0) {
        u64 m = 0x000fffffffffffff >> e;
        if ((u.i & m) == 0) return x; // fractional part is zero
        if (u.i >> 63 == 0) u.i += m; // the actual rounding
        u.i &= ~m;
    } else {
        if (u.i >> 63) u.f = -0.0;
        else if (u.i << 1) u.f = 1.0;
    }

    return u.f;
}

#pragma endregion

#pragma region Min/Max/Clamp -----------------------------------------------------------------------------------------

template <typename TFloat>
constexpr TFloat max(TFloat a, TFloat b) {
    if (a < b) return b;
    return a;
}

template <typename TFloat>
constexpr TFloat min(TFloat a, TFloat b) {
    if (a > b) return b;
    return a;
}

template <typename TFloat>
constexpr tuple<TFloat, TFloat> minmax(TFloat a, TFloat b) {
    return core::create_tuple(core::min(a, b), core::max(a, b));
}

template <typename TFloat>
inline TFloat clamp(TFloat value, TFloat min, TFloat max) {
    return core::max(min, core::min(max, value));
}

#pragma endregion

#pragma region Abs/Sign -----------------------------------------------------------------------------------------------

inline f32 abs(f32 a) {
    // NOTE: This is pretty fast branchless check. Its collapsed to a single instruction on x86 and ARM by most compilers.
    i32* ip = reinterpret_cast<i32*>(&a);
    *ip &= 0x7fffffff;
    return a;
}

inline f64 abs(f64 a) {
    i64* ip = reinterpret_cast<i64*>(&a);
    *ip &= 0x7fffffffffffffff;
    return a;
}

template <typename T>
constexpr T abs_slow(T a) {
    // can be done branchless, but it's not faster.
    return a < 0 ? -a : a;
}

#pragma endregion

#pragma region IsPositive ---------------------------------------------------------------------------------------------

inline bool is_positive(f32 a) {
    // NOTE: This is pretty fast branchless check. Its collapsed to a single instruction on x86 and ARM by most compilers.
    i32* ip = reinterpret_cast<i32*>(&a);
    *ip = (*ip >> 31) << 1;
    return *ip == 0;
}

inline bool is_positive(f64 a) {
    i64* ip = reinterpret_cast<i64*>(&a);
    *ip = (*ip >> 63) << 1;
    return *ip == 0;
}

constexpr bool is_positive(i8 a) {
    u32 temp = a >> (sizeof(i8) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i16 a) {
    u32 temp = a >> (sizeof(i16) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i32 a) {
    u32 temp = a >> (sizeof(i32) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i64 a) {
    u64 temp = a >> (sizeof(i64) * 8 - 1);
    return temp == 0;
}

#pragma endregion

#pragma region Safe Comparisons ---------------------------------------------------------------------------------------

/**
 * @brief Safely very basic epsilon comparison.
*/
inline bool safe_eq(f32 a, f32 b, f32 epsilon) {
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Safely very basic epsilon comparison.
*/
inline bool safe_eq(f64 a, f64 b, f64 epsilon) {
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Compares two floating point numbers for being nearly equal, depending on the given epsilon.
 *        This is much more precise than the safe_eq function, but also significantly slower.
*/
inline bool nearly_eq(f32 a, f32 b, f32 epsilon) {
    if (a == b) return true;
    f32 absA = core::abs(a);
    f32 absB = core::abs(b);
    f32 diff = core::abs(absA - absB);
    if (a == 0 || b == 0 || (absA + absB < MIN_NORMAL_F32)) {
        return diff < (epsilon * MIN_NORMAL_F32);
    }
    return diff / core::min((absA + absB), MAX_F32) < epsilon;
}

/**
 * @brief Compares two floating point numbers for being nearly equal, depending on the given epsilon.
 *        This is much more precise than the safe_eq function, but also significantly slower.
*/
inline bool nearly_eq(f64 a, f64 b, f64 epsilon) {
    if (a == b) return true;
    f64 absA = core::abs(a);
    f64 absB = core::abs(b);
    f64 diff = core::abs(absA - absB);
    if (a == 0 || b == 0 || (absA + absB < MIN_NORMAL_F64)) {
        return diff < (epsilon * MIN_NORMAL_F64);
    }
    return diff / core::min((absA + absB), MAX_F64) < epsilon;
}

#pragma endregion

#pragma region Interpolation ------------------------------------------------------------------------------------------

/**
 * @brief Performs an affine transformation on a value from one 2d range to another.
 *
 * @param x1 The start of the first range.
 * @param y1 The end of the first range.
 * @param x2 The start of the second range.
 * @param y2 The end of the second range.
 * @param t The value to transform.
 *
 * @return The transformed value.
*/
template <typename TFloat>
inline TFloat map2DRange(TFloat x1, TFloat y1, TFloat x2, TFloat y2, TFloat t) {
    Assert((y1 - x1) != 0, "division by zero");
    return ((t - x1) * (y2 - x2)) / (y1 - x1) + x2;
}

/**
 * @brief Performs a precise monotonic linear interpolation between two values.
 *        When t = 1, b is returned.
 *
 * @param a The start value.
 * @param b The end value.
 * @param t The interpolation factor.
 *
 * @return The interpolated value.
*/
template <typename TFloat>
inline TFloat lerp(TFloat a, TFloat b, TFloat t) {
    return (1 - t) * a + t * b;
}

/**
 * @brief Performs an imprecise linear interpolation between two values.
 *        This does not guarantee that when t = 1 that b is returned.
 *
 * @param a The start value.
 * @param b The end value.
 * @param t The interpolation factor.
 *
 * @return The interpolated value.
*/
template <typename TFloat>
inline TFloat lerp_fast(TFloat a, TFloat b, TFloat t) {
    return a + t * (b - a);
}

#pragma endregion

} // namespace core
