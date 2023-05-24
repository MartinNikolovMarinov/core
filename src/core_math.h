#pragma once

#include <API.h>
#include <types.h>
#include <tuple.h>

namespace core {

using namespace coretypes;

constexpr u64 pow_u64(i64 n, u32 p) {
    // TODO: Implement faster. I could use some approximating algorithm for very fast pow function.
    //       This might actually be worse than using a for loop?
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

template <typename T>
constexpr i32 sign_slow(T n) {
    return n < 0 ? -1 : 1;
}

constexpr i32 sign(f32 n) {
    union { f32 f; u32 i; } u = { n };
    return u.i >> 31;
}

constexpr i32 sign(f64 n) {
    union { f64 f; u64 i; } u = { n };
    return u.i >> 63;
}

template <typename TFloat>
constexpr TFloat round(TFloat n) {
    return n < 0.0f ? n - 0.5f : n + 0.5f;
}

template <typename TFloat>
constexpr TFloat round_to(TFloat n, u32 to) {
    return TFloat(round(n * TFloat(10*to))) / TFloat(10*to);
}

template <typename TFloat>
constexpr TFloat floor(TFloat n) {
    return n < 0.0f ? n - 1.0f : n;
}

template <typename TFloat>
constexpr TFloat ceil(TFloat n) {
    return n < 0.0f ? n : n + 1.0f;
}

constexpr f32 deg_to_rad(f32 n) {
    return n * (PI / 180.0f);
}

constexpr f32 rad_to_deg(f32 n) {
    return n * (180.0f / PI);
}

template <typename T>
constexpr T max(T a, T b) {
    // can be done branchless, but it's not faster.
    return a > b ? a : b;
}

template <typename T>
constexpr T min(T a, T b) {
    // can be done branchless, but it's not faster.
    return a < b ? a : b;
}

template <typename T>
constexpr tuple<T, T> minmax(T a, T b) {
    return a < b ? tuple<T, T>(a, b) : tuple<T, T>(b, a);
}

template <typename T>
inline T clamp(T value, T min, T max) {
    return core::max(min, core::min(max, value));
}

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
constexpr T abs(T a) {
    // can be done branchless, but it's not faster.
    return a < 0 ? -a : a;
}

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

/**
 * @brief Safely compare two floats for being equal. It's a very basic epsilon comparison.
*/
inline bool safe_eq(f32 a, f32 b, f32 epsilon) {
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Safely compare two floats for being equal. It's a very basic epsilon comparison.
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

} // namespace core
