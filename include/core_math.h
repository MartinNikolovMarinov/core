#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_tuple.h>
#include <core_traits.h>
#include <core_system_checks.h>
#include <core_utils.h>

namespace core {

using namespace coretypes;

#pragma region Limit Helper Functions ---------------------------------------------------------------------------------

template <typename T>
constexpr T limitMax() {
    if constexpr (core::is_same_v<T, u8>)       return core::MAX_U8;
    else if constexpr (core::is_same_v<T, u16>) return core::MAX_U16;
    else if constexpr (core::is_same_v<T, u32>) return core::MAX_U32;
    else if constexpr (core::is_same_v<T, u64>) return core::MAX_U64;
    else if constexpr (core::is_same_v<T, i8>)  return core::MAX_I8;
    else if constexpr (core::is_same_v<T, i16>) return core::MAX_I16;
    else if constexpr (core::is_same_v<T, i32>) return core::MAX_I32;
    else if constexpr (core::is_same_v<T, i64>) return core::MAX_I64;
    else if constexpr (core::is_same_v<T, f32>) return core::MAX_F32;
    else if constexpr (core::is_same_v<T, f64>) return core::MAX_F64;
    else static_assert(core::always_false<T>, "Unsupported type");
}

template <typename T>
constexpr T limitMin() {
    if constexpr (core::is_same_v<T, u8>)       return 0;
    else if constexpr (core::is_same_v<T, u16>) return 0;
    else if constexpr (core::is_same_v<T, u32>) return 0;
    else if constexpr (core::is_same_v<T, u64>) return 0;
    else if constexpr (core::is_same_v<T, i8>)  return core::MIN_I8;
    else if constexpr (core::is_same_v<T, i16>) return core::MIN_I16;
    else if constexpr (core::is_same_v<T, i32>) return core::MIN_I32;
    else if constexpr (core::is_same_v<T, i64>) return core::MIN_I64;
    else if constexpr (core::is_same_v<T, f32>) return core::MIN_NORMAL_F32;
    else if constexpr (core::is_same_v<T, f64>) return core::MIN_NORMAL_F64;
    else static_assert(core::always_false<T>, "Unsupported type");
}

#pragma endregion

#pragma region Powers of 10 and 2 -------------------------------------------------------------------------------------

namespace detail {

static constexpr u64 powersOf10LookupTable[] = {
    1ULL,
    10ULL,
    100ULL,
    1000ULL,
    10000ULL,
    100000ULL,
    1000000ULL,
    10000000ULL,
    100000000ULL,
    1000000000ULL,
    10000000000ULL,
    100000000000ULL,
    1000000000000ULL,
    10000000000000ULL,
    100000000000000ULL,
    1000000000000000ULL,
    10000000000000000ULL,
    100000000000000000ULL,
    1000000000000000000ULL,
    10000000000000000000ULL,
};

} // namespace detail

constexpr u64 pow10(u32 i) {
    Assert(i < sizeof(detail::powersOf10LookupTable) / sizeof(u64));
    return detail::powersOf10LookupTable[i];
}

constexpr u64 pow2(u32 i) {
    Assert(i < 64);
    return 1ULL << i;
}

#pragma endregion

#pragma region align to power of 2 ------------------------------------------------------------------------------------

namespace detail {

template <typename TInt>
constexpr TInt alignToPow2(TInt v) {
    if (v == 0) return 0;

    // Decrement by 1 so if the number is already a power of 2, it won't go up to the next power
    v--;

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;

    if constexpr (sizeof(TInt) >= 2) {
        v |= v >> 8;
    }
    if constexpr (sizeof(TInt) >= 4) {
        v |= v >> 16;
    }
    if constexpr (sizeof(TInt) >= 8) {
        v |= v >> 32;
    }

    // Increment by 1 because of the previous decrement.
    return v + 1;
}

} // namespace detail

constexpr i8 alignToPow2(i8 v)   { return detail::alignToPow2(v); }
constexpr u8 alignToPow2(u8 v)   { return detail::alignToPow2(v); }
constexpr i16 alignToPow2(i16 v) { return detail::alignToPow2(v); }
constexpr u16 alignToPow2(u16 v) { return detail::alignToPow2(v); }
constexpr i32 alignToPow2(i32 v) { return detail::alignToPow2(v); }
constexpr u32 alignToPow2(u32 v) { return detail::alignToPow2(v); }
constexpr i64 alignToPow2(i64 v) { return detail::alignToPow2(v); }
constexpr u64 alignToPow2(u64 v) { return detail::alignToPow2(v); }

#pragma endregion

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
static_assert(core::is_trivial_v<radians>);
static_assert(sizeof(radians) == sizeof(f32));

constexpr radians degToRad(f32 n) {
    return radians(n * (PI / 180.0f));
}

constexpr f32 radToDeg(const radians& n) {
    return n.value * (180.0f / PI);
}

#pragma endregion

#pragma region Take Exponent/Mantisssa --------------------------------------------------------------------------------

// NOTE:
// The following code takes the exponent and mantissa of a floating point number, which is encoded in the IEEE-754.
// The code does not support little-endian systems. Support should not be too difficult if I need it.
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
// The value of a IEEE-754 number is computed as: sign * 2^exponent * mantissa.

CORE_API_EXPORT u32 exponent(f32 n);
CORE_API_EXPORT u32 exponent(f64 n);
CORE_API_EXPORT u32 mantissa(f32 n);
CORE_API_EXPORT u64 mantissa(f64 n);

#pragma endregion

#pragma region Min/Max/Clamp -----------------------------------------------------------------------------------------

template <typename T>
constexpr T max(T a, T b) {
    if (a < b) return b;
    return a;
}

template <typename T>
constexpr T min(T a, T b) {
    if (a > b) return b;
    return a;
}

template <typename T>
constexpr tuple<T, T> minmax(T a, T b) {
    return core::create_tuple(core::min(a, b), core::max(a, b));
}

template <typename T>
constexpr T clamp(T value, T min, T max) {
    return core::max(min, core::min(max, value));
}

#pragma endregion

#pragma region Abs/Sign -----------------------------------------------------------------------------------------------

// TODO2: [Nit Pick] absSlow is a bad name for ths function. It's not slow, it's just not branchless.
template <typename T>
constexpr T absSlow(T a) {
    // can be done branchless, but it's not faster.
    return a < 0 ? -a : a;
}

constexpr f32 abs(f32 a) {
    IS_CONST_EVALUATED { return absSlow(a); }
    else {
        // NOTE: This is pretty fast branchless check. It's collapsed to a single instruction on x86 and ARM by most compilers.
        i32* ip = reinterpret_cast<i32*>(&a);
        *ip &= 0x7fffffff;
        return a;
    }
}

constexpr f64 abs(f64 a) {
    IS_CONST_EVALUATED { return absSlow(a); }
    else {
        i64* ip = reinterpret_cast<i64*>(&a);
        *ip &= 0x7fffffffffffffff;
        return a;
    }
}

constexpr i8  abs(i8 a)  { return a < i8(0) ? i8(-a) : i8(a); }
constexpr i16 abs(i16 a) { return a < i16(0) ? i16(-a) : i16(a); }
constexpr i32 abs(i32 a) { return a < i32(0) ? i32(-a) : i32(a); }
constexpr i64 abs(i64 a) { return a < i64(0) ? i64(-a) : i64(a); }

#pragma endregion

#pragma region IsPositive ---------------------------------------------------------------------------------------------

constexpr bool isPositive(f32 a) {
    IS_CONST_EVALUATED { return a >= 0; }
    else {
        // NOTE: This is pretty fast branchless check. It's collapsed to a single instruction on x86 and ARM by most compilers.
        i32* ip = reinterpret_cast<i32*>(&a);
        *ip = (*ip >> 31) << 1;
        return *ip == 0;
    }
}

constexpr bool isPositive(f64 a) {
    IS_CONST_EVALUATED { return a >= 0; }
    else {
        i64* ip = reinterpret_cast<i64*>(&a);
        *ip = (*ip >> 63) << 1;
        return *ip == 0;
    }
}

constexpr bool isPositive(i8 a) {
    i32 temp = a >> (sizeof(i8) * 8 - 1);
    return temp == 0;
}

constexpr bool isPositive(i16 a) {
    i32 temp = a >> (sizeof(i16) * 8 - 1);
    return temp == 0;
}

constexpr bool isPositive(i32 a) {
    i32 temp = a >> (sizeof(i32) * 8 - 1);
    return temp == 0;
}

constexpr bool isPositive(i64 a) {
    i64 temp = a >> (sizeof(i64) * 8 - 1);
    return temp == 0;
}

#pragma endregion

#pragma region Safe Comparisons ---------------------------------------------------------------------------------------

/**
 * @brief Safely very basic epsilon comparison.
 *        Passing NaN or infinity to any of the arguments is undefined behaviour.
*/
constexpr bool safeEq(f32 a, f32 b, f32 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Safely very basic epsilon comparison.
 *        Passing NaN or infinity to any of the arguments is undefined behaviour.
*/
constexpr bool safeEq(f64 a, f64 b, f64 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Compares two floating point numbers for being nearly equal, depending on the given epsilon.
 *        This is much more precise than the safeEq function, but also significantly slower.
 *        Passing NaN or infinity to any of the arguments is undefined behaviour.
*/
constexpr bool nearlyEq(f32 a, f32 b, f32 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
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
 *        This is much more precise than the safeEq function, but also significantly slower.
 *        Passing NaN or infinity to any of the arguments is undefined behaviour.
*/
constexpr bool nearlyEq(f64 a, f64 b, f64 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
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
 * @brief Performs an affine transformation on a value from one range to another.
 *
 * @param v The value to map. This should be in the range [fromMin, fromMax].
 * @param fromMax The maximum value of the input range.
 * @param fromMin The minimum value of the input range.
 * @param toMax The maximum value of the output range.
 * @param toMin The minimum value of the output range.
 *
 * @return The mapped value.
*/
template <typename T>
constexpr T affineMap(T v, T fromMin, T fromMax, T toMin, T toMax) {
    T fromRange = fromMax - fromMin;
    T toRange = toMax - toMin;
    T relativeLoc = (v - fromMin) / fromRange;
    T mappedPos = relativeLoc * toRange + toMin;
    return mappedPos;
}

/**
 * @brief Performs a precise monotonic linear interpolation between two values.
 *        When t = 1, b is returned.
 *
 * @param a The start value.
 * @param b The end value.
 * @param t The interpolation factor. Can be a math object or a floating point value.
 *
 * @return The interpolated value.
*/
template <typename T, typename T2>
constexpr T lerp(T a, T b, T2 t) {
    return (1 - t) * a + t * b;
}

/**
 * @brief Performs an imprecise linear interpolation between two values.
 *        This does not guarantee that when t = 1 that b is returned.
 *
 * @param a The start value.
 * @param b The end value.
 * @param t The interpolation factor. Can be a math object or a floating point value.
 *
 * @return The interpolated value.
*/
template <typename T, typename T2>
constexpr T lerpFast(T a, T b, T2 t) {
    return a + t * (b - a);
}

#pragma endregion

} // namespace core