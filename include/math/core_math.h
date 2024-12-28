#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_bits.h>
#include <core_intrinsics.h>
#include <core_system_checks.h>
#include <core_traits.h>
#include <core_tuple.h>
#include <core_types.h>

#include <cmath>

namespace core {

using namespace coretypes;

#pragma region Forward Declarations -----------------------------------------------------------------------------------

struct radians;

template <typename TFloat>          constexpr u32         exponentBits();
template <typename TFloat>          constexpr u32         exponentBias();
template <typename TFloat>          constexpr u32         mantissaBits();
template <typename TFloat>          constexpr u32         maxMantissaDigitsBase10();
                                    constexpr f32         createFloat32(u32 mantissa, u32 exponent, bool sign);
                                    constexpr f64         createFloat64(u64 mantissa, u64 exponent, bool sign);
                                    constexpr void        decomposeFloat32(f32 n, u32& mantissa, u32& exponent, bool& sign);
                                    constexpr void        decomposeFloat64(f64 n, u64& mantissa, u32& exponent, bool& sign);

template <typename TInt>            constexpr i32         maxDigitsBase2();
template <typename TInt>            constexpr i32         maxDigitsBase10();

template <typename T>               constexpr T           absGeneric(T a);
                                    constexpr f32         abs(f32 a);
                                    constexpr f64         abs(f64 a);

                                    constexpr f32         infinityF32();
                                    constexpr f32         quietNaNF32();
                                    constexpr f32         signalingNaNF32();
                                    constexpr f64         infinityF64();
                                    constexpr f64         quietNaNF64();
                                    constexpr f64         signalingNaNF64();

                                    constexpr bool        isinf(f32 x);
                                    constexpr bool        isinf(f64 x);
                                    constexpr bool        isnan(f32 x);
                                    constexpr bool        isnan(f64 x);
                                    constexpr bool        isnormal(f32 x);
                                    constexpr bool        isnormal(f64 x);
                                    constexpr i32         fpclassify(f32 x);
                                    constexpr i32         fpclassify(f64 x);

                                    constexpr f32         modf(f32 x, f32* iptr);
                                    constexpr f64         modf(f64 x, f64* iptr);

                                    constexpr f32         floor(f32 x);
                                    constexpr f64         floor(f64 x);
                                    constexpr f32         ceil(f32 x);
                                    constexpr f64         ceil(f64 x);
                                    constexpr f32         trunc(f32 x);
                                    constexpr f64         trunc(f64 x);
                                    constexpr f32         round(f32 x);
                                    constexpr f64         round(f64 x);
                                    constexpr f32         roundTo(f32 n, u32 to);
                                    constexpr f64         roundTo(f64 n, u32 to);

                                    constexpr f32         sqrt(f32 x);
                                    constexpr f64         sqrt(f64 x);
                                    inline f32            pow(f32 x, f32 exp);
                                    inline f64            pow(f64 x, f64 exp);
                                    inline f32            sin(f32 x);
                                    inline f64            sin(f64 x);
                                    inline f32            cos(f32 x);
                                    inline f64            cos(f64 x);
                                    inline f32            tan(f32 x);
                                    inline f64            tan(f64 x);
                                    inline f32            asin(f32 x);
                                    inline f64            asin(f64 x);
                                    inline f32            acos(f32 x);
                                    inline f64            acos(f64 x);
                                    inline f32            atan(f32 x);
                                    inline f64            atan(f64 x);
                                    inline f32            atan2(f32 a, f32 b);
                                    inline f64            atan2(f64 a, f64 b);
                                    constexpr u64         pow10(u32 i);
                                    constexpr u64         pow2(u32 i);

                                    constexpr i8          alignToPow2(i8 v);
                                    constexpr u8          alignToPow2(u8 v);
                                    constexpr i16         alignToPow2(i16 v);
                                    constexpr u16         alignToPow2(u16 v);
                                    constexpr i32         alignToPow2(i32 v);
                                    constexpr u32         alignToPow2(u32 v);
                                    constexpr i64         alignToPow2(i64 v);
                                    constexpr u64         alignToPow2(u64 v);

                                    constexpr f64         piF64();
                                    constexpr f32         piF32();
                                    constexpr f64         pi();

                                    constexpr radians     degToRad(f32 n);
                                    constexpr f32         radToDeg(const radians& n);

template <typename T>               constexpr T           core_max(const T& a, const T& b);
template <typename T>               constexpr T           core_min(const T& a, const T& b);
template <typename T>               constexpr tuple<T, T> minmax(const T& a, const T& b);
template <typename T>               constexpr T           clamp(const T& value, const T& min, const T& max);

                                    constexpr bool        isPositive(f32 a);
                                    constexpr bool        isPositive(f64 a);
                                    constexpr bool        isPositive(i8 a);
                                    constexpr bool        isPositive(i16 a);
                                    constexpr bool        isPositive(i32 a);
                                    constexpr bool        isPositive(i64 a);

                                    constexpr bool        safeEq(f32 a, f32 b, f32 epsilon);
                                    constexpr bool        safeEq(f64 a, f64 b, f64 epsilon);
                                    constexpr bool        nearlyEq(f32 a, f32 b, f32 epsilon);
                                    constexpr bool        nearlyEq(f64 a, f64 b, f64 epsilon);

                                    constexpr bool         safeAdd(i8 a, i8 b, i8& out);
                                    constexpr bool         safeAdd(u8 a, u8 b, u8& out);
                                    constexpr bool         safeAdd(i16 a, i16 b, i16& out);
                                    constexpr bool         safeAdd(u16 a, u16 b, u16& out);
                                    constexpr bool         safeAdd(i32 a, i32 b, i32& out);
                                    constexpr bool         safeAdd(u32 a, u32 b, u32& out);
                                    constexpr bool         safeAdd(i64 a, i64 b, i64& out);
                                    constexpr bool         safeAdd(u64 a, u64 b, u64& out);

                                    constexpr bool         safeSub(i8 a, i8 b, i8& out);
                                    constexpr bool         safeSub(u8 a, u8 b, u8& out);
                                    constexpr bool         safeSub(i16 a, i16 b, i16& out);
                                    constexpr bool         safeSub(u16 a, u16 b, u16& out);
                                    constexpr bool         safeSub(i32 a, i32 b, i32& out);
                                    constexpr bool         safeSub(u32 a, u32 b, u32& out);
                                    constexpr bool         safeSub(i64 a, i64 b, i64& out);
                                    constexpr bool         safeSub(u64 a, u64 b, u64& out);

                                    constexpr bool         safeMul(i8 a, i8 b, i8& out);
                                    constexpr bool         safeMul(u8 a, u8 b, u8& out);
                                    constexpr bool         safeMul(i16 a, i16 b, i16& out);
                                    constexpr bool         safeMul(u16 a, u16 b, u16& out);
                                    constexpr bool         safeMul(i32 a, i32 b, i32& out);
                                    constexpr bool         safeMul(u32 a, u32 b, u32& out);
                                    constexpr bool         safeMul(i64 a, i64 b, i64& out);
                                    constexpr bool         safeMul(u64 a, u64 b, u64& out);

                                    constexpr bool         safeDiv(i8 a, i8 b, i8& out);
                                    constexpr bool         safeDiv(u8 a, u8 b, u8& out);
                                    constexpr bool         safeDiv(i16 a, i16 b, i16& out);
                                    constexpr bool         safeDiv(u16 a, u16 b, u16& out);
                                    constexpr bool         safeDiv(i32 a, i32 b, i32& out);
                                    constexpr bool         safeDiv(u32 a, u32 b, u32& out);
                                    constexpr bool         safeDiv(i64 a, i64 b, i64& out);
                                    constexpr bool         safeDiv(u64 a, u64 b, u64& out);

template <typename T>               constexpr T           affineMap(T v, T fromMin, T fromMax, T toMin, T toMax);
template <typename T, typename T2>  constexpr T           lerp(T a, T b, T2 t);
template <typename T, typename T2>  constexpr T           lerpFast(T a, T b, T2 t);

#pragma endregion

#pragma region Mantissa and Exponent

template <typename TFloat> constexpr u32 exponentBits() {
    if constexpr (std::is_same_v<TFloat, f32>) return 8;
    else if constexpr (std::is_same_v<TFloat, f64>) return 11;
    else static_assert(core::always_false<TFloat>, "Unsupported type");
}

template <typename TFloat> constexpr u32 exponentBias() {
    if constexpr (std::is_same_v<TFloat, f32>) return 127;
    else if constexpr (std::is_same_v<TFloat, f64>) return 1023;
    else static_assert(core::always_false<TFloat>, "Unsupported type");
}

template <typename TFloat> constexpr u32 mantissaBits() {
    if constexpr (std::is_same_v<TFloat, f32>) return 23;
    else if constexpr (std::is_same_v<TFloat, f64>) return 52;
    else static_assert(core::always_false<TFloat>, "Unsupported type");
}

template <typename TFloat> constexpr u32 maxMantissaDigitsBase10() {
    if constexpr (std::is_same_v<TFloat, f32>) return 9;
    else if constexpr (std::is_same_v<TFloat, f64>) return 17;
    else static_assert(core::always_false<TFloat>, "Unsupported type");
}

constexpr f32 createFloat32(u32 mantissa, u32 exponent, bool sign) {
    constexpr u32 SIGN_BIT_SHIFT = 31;
    constexpr u32 EXPONENT_BIT_SHIFT = 23;
    constexpr u32 EXPONENT_MASK = 0xFF;       // 8 bits for exponent
    constexpr u32 MANTISSA_MASK = 0x7FFFFF;  // 23 bits for mantissa

    u32 signBit = sign ? 1u : 0u;
    u32 ieee754 = (signBit << SIGN_BIT_SHIFT)       // Sign bit (1 bit)
                | ((exponent & EXPONENT_MASK) << EXPONENT_BIT_SHIFT) // Exponent (8 bits)
                | (mantissa & MANTISSA_MASK);      // Mantissa (23 bits)
    return core::bitCast<f32>(ieee754);
}

constexpr f64 createFloat64(u64 mantissa, u64 exponent, bool sign) {
    constexpr u64 SIGN_BIT_SHIFT = 63;
    constexpr u64 EXPONENT_BIT_SHIFT = 52;
    constexpr u64 EXPONENT_MASK = 0x7FF;          // 11 bits for exponent
    constexpr u64 MANTISSA_MASK = 0xFFFFFFFFFFFFF; // 52 bits for mantissa

    u64 signBit = sign ? 1u : 0u;
    u64 ieee754 = (signBit << SIGN_BIT_SHIFT)       // Sign bit (1 bit)
                | ((exponent & EXPONENT_MASK) << EXPONENT_BIT_SHIFT) // Exponent (11 bits)
                | (mantissa & MANTISSA_MASK);      // Mantissa (52 bits)
    return core::bitCast<f64>(ieee754);
}

constexpr void decomposeFloat32(f32 n, u32& mantissa, u32& exponent, bool& sign) {
    constexpr u32 SIGN_BIT_SHIFT = 31;
    constexpr u32 EXPONENT_BIT_SHIFT = 23;
    constexpr u32 EXPONENT_MASK = 0xFF;       // 8 bits for exponent
    constexpr u32 MANTISSA_MASK = 0x7FFFFF;  // 23 bits for mantissa

    u32 bits = core::bitCast<u32>(n);
    sign = ((bits >> SIGN_BIT_SHIFT) & 1u) != 0u;
    exponent = (bits >> EXPONENT_BIT_SHIFT) & EXPONENT_MASK;
    mantissa = bits & MANTISSA_MASK;
}

constexpr void decomposeFloat64(f64 n, u64& mantissa, u32& exponent, bool& sign) {
    constexpr u64 SIGN_BIT_SHIFT = 63;
    constexpr u64 EXPONENT_BIT_SHIFT = 52;
    constexpr u64 EXPONENT_MASK = 0x7FF;           // 11 bits for exponent
    constexpr u64 MANTISSA_MASK = 0xFFFFFFFFFFFFF; // 52 bits for mantissa

    u64 bits = core::bitCast<u64>(n);
    sign = ((bits >> SIGN_BIT_SHIFT) & 1u) != 0u;
    exponent = u32((bits >> EXPONENT_BIT_SHIFT) & EXPONENT_MASK);
    mantissa = bits & MANTISSA_MASK;
}

#pragma endregion

#pragma region Max Digits ---------------------------------------------------------------------------------------------

template <typename TInt>
constexpr i32 maxDigitsBase2() {
    if constexpr (std::is_same_v<TInt, u8>)         return core::BYTE_SIZE;
    else if constexpr (std::is_same_v<TInt, u16>)   return core::BYTE_SIZE * sizeof(u16);
    else if constexpr (std::is_same_v<TInt, u32>)   return core::BYTE_SIZE * sizeof(u32);
    else if constexpr (std::is_same_v<TInt, u64>)   return core::BYTE_SIZE * sizeof(u64);
    else if constexpr (std::is_same_v<TInt, i8>)    return core::BYTE_SIZE * sizeof(i8) - 1;
    else if constexpr (std::is_same_v<TInt, i16>)   return core::BYTE_SIZE * sizeof(i16) - 1;
    else if constexpr (std::is_same_v<TInt, i32>)   return core::BYTE_SIZE * sizeof(i32) - 1;
    else if constexpr (std::is_same_v<TInt, i64>)   return core::BYTE_SIZE * sizeof(i64) - 1;
    else if constexpr (std::is_same_v<TInt, char>)  return core::BYTE_SIZE * sizeof(char);
    else if constexpr (std::is_same_v<TInt, uchar>) return core::BYTE_SIZE * sizeof(uchar);
    else if constexpr (std::is_same_v<TInt, schar>) return core::BYTE_SIZE * sizeof(schar) - 1;
    else static_assert(core::always_false<TInt>, "Unsupported type");
}

template <typename TInt>
constexpr i32 maxDigitsBase10() {
    if constexpr (std::is_same_v<TInt, u8>)         return 3;
    else if constexpr (std::is_same_v<TInt, u16>)   return 5;
    else if constexpr (std::is_same_v<TInt, u32>)   return 10;
    else if constexpr (std::is_same_v<TInt, u64>)   return 20;
    else if constexpr (std::is_same_v<TInt, i8>)    return 4;
    else if constexpr (std::is_same_v<TInt, i16>)   return 6;
    else if constexpr (std::is_same_v<TInt, i32>)   return 11;
    else if constexpr (std::is_same_v<TInt, i64>)   return 20;
    else if constexpr (std::is_same_v<TInt, char>)  return 4;
    else if constexpr (std::is_same_v<TInt, uchar>) return 3;
    else if constexpr (std::is_same_v<TInt, schar>) return 4;
    else static_assert(core::always_false<TInt>, "Unsupported type");
}

#pragma endregion

#pragma region Abs/Sign -----------------------------------------------------------------------------------------------

template <typename T>
constexpr T absGeneric(T a) {
    return T(a) < T(0) ? T(-a) : T(a);
}

constexpr f32 abs(f32 a) {
    IS_CONST_EVALUATED { return core::absGeneric(a); }
    else {
        // NOTE: This is pretty fast branchless check. It's collapsed to a single instruction on x86 and ARM by most compilers.
        i32* ip = reinterpret_cast<i32*>(&a);
        *ip &= 0x7fffffff;
        return a;
    }
}

constexpr f64 abs(f64 a) {
    IS_CONST_EVALUATED { return core::absGeneric(a); }
    else {
        // NOTE: This is pretty fast branchless check. It's collapsed to a single instruction on x86 and ARM by most compilers.
        i64* ip = reinterpret_cast<i64*>(&a);
        *ip &= 0x7fffffffffffffff;
        return a;
    }
}

#pragma endregion

#pragma region Classification -----------------------------------------------------------------------------------------

constexpr f32 infinityF32()     { return core::intrin_hugeValf(); }
constexpr f32 quietNaNF32()     { return core::intrin_nanf(); }
constexpr f32 signalingNaNF32() { return core::intrin_nansf(); }

constexpr f64 infinityF64()     { return core::intrin_hugeVal(); }
constexpr f64 quietNaNF64()     { return core::intrin_nan(); }
constexpr f64 signalingNaNF64() { return core::intrin_nans(); }

template <typename T> constexpr T infinity() {
    static_assert(std::is_floating_point_v<T>);
    if constexpr (sizeof(T) == 4) return infinityF32();
    else if constexpr (sizeof(T) == 8) return infinityF64();
    else static_assert(core::always_false<T>());
}

template <typename T> constexpr T quietNaN() {
    static_assert(std::is_floating_point_v<T>);
    if constexpr (sizeof(T) == 4) return quietNaNF32();
    else if constexpr (sizeof(T) == 8) return quietNaNF64();
    else static_assert(core::always_false<T>());
}

template <typename T> constexpr T signalingNaN() {
    static_assert(std::is_floating_point_v<T>);
    if constexpr (sizeof(T) == 4) return signalingNaNF32();
    else if constexpr (sizeof(T) == 8) return signalingNaNF64();
    else static_assert(core::always_false<T>());
}

namespace detail {

constexpr bool isinfCompiletimeImpl(f32 x) {
    return x == infinityF32() || -x == infinityF32();
}

constexpr bool isinfCompiletimeImpl(f64 x) {
    return x == infinityF64() || -x == infinityF64();
}

template <typename TFloat>
constexpr bool isinf(TFloat x) {
    IS_CONST_EVALUATED { return detail::isinfCompiletimeImpl(x); }
    return std::isinf(x);
}

} // namespace detail

constexpr bool isinf(f32 x) { return detail::isinf(x); }
constexpr bool isinf(f64 x) { return detail::isinf(x); }

namespace detail {

template <typename TFloat>
constexpr bool isnanCompiletimeImpl(TFloat x) {
    return x != x;
}

template <typename TFloat>
constexpr bool isnan(TFloat x) {
    IS_CONST_EVALUATED { return detail::isnanCompiletimeImpl(x); }
    return std::isnan(x);
}

} // namespace detail

constexpr bool isnan(f32 x) { return detail::isnan(x); }
constexpr bool isnan(f64 x) { return detail::isnan(x); }

namespace detail {

template <typename TFloat>
constexpr bool isnormalCompiletimeImpl(TFloat x) {
    bool res = x != 0 &&
               !detail::isinfCompiletimeImpl(x) &&
               !detail::isnanCompiletimeImpl(x) &&
                core::abs(x) >= core::limitMin<TFloat>();
    return res;
}

template <typename TFloat>
constexpr bool isnormal(TFloat x) {
    IS_CONST_EVALUATED { return detail::isnormalCompiletimeImpl(x); }
    return std::isnormal(x);
}

} // namespace detail

constexpr bool isnormal(f32 x) { return detail::isnormal(x); }
constexpr bool isnormal(f64 x) { return detail::isnormal(x); }

namespace detail {

template <typename TFloat>
constexpr i32 fpclassifyCompiletimeImpl(TFloat x) {
    if (detail::isnanCompiletimeImpl(x))                              return FP_NAN;
    if (detail::isinfCompiletimeImpl(x))                              return FP_INFINITE;
    if (core::abs(x) == TFloat(0))                                    return FP_ZERO;
    if (core::abs(x) > 0 && core::abs(x) < core::limitMin<TFloat>())  return FP_SUBNORMAL;
    return FP_NORMAL;
}

template <typename TFloat>
constexpr i32 fpclassify(TFloat x) {
    IS_CONST_EVALUATED { return fpclassifyCompiletimeImpl(x); }
    return std::fpclassify(x);
}

} // namespace detail

constexpr i32 fpclassify(f32 x) { return detail::fpclassify(x); }
constexpr i32 fpclassify(f64 x) { return detail::fpclassify(x); }

#pragma endregion

#pragma region Modf --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat> constexpr TFloat truncCompiletimeImpl(TFloat x);

template <typename TFloat>
constexpr TFloat modfErrorImpl(TFloat x, TFloat* iptr) {
    *iptr = x;
    if (core::abs(x) == TFloat(0)) return x;
    return x > TFloat(0) ? TFloat(0) : -TFloat(0);
}

template <typename TFloat>
constexpr TFloat modfNanImpl(TFloat x, TFloat* iptr) {
    *iptr = x;
    return x;
}

template <typename TFloat>
constexpr TFloat modfCompiletimeImpl(TFloat x, TFloat* iptr) {
    if (detail::isnanCompiletimeImpl(x)) return modfNanImpl(x, iptr);
    if (detail::isinfCompiletimeImpl(x)) return modfErrorImpl(x, iptr);
    if (core::abs(x) == TFloat(0))       return modfErrorImpl(x, iptr);

    *iptr = core::detail::truncCompiletimeImpl(x);
    return (x - *iptr);
}

template <typename TFloat>
constexpr TFloat modf(TFloat x, TFloat* iptr) {
    IS_CONST_EVALUATED { return modfCompiletimeImpl(x, iptr); }
    return std::modf(x, iptr);
}

} // namespace detail

constexpr f32 modf(f32 x, f32* iptr) { return detail::modf(x, iptr); }
constexpr f64 modf(f64 x, f64* iptr) { return detail::modf(x, iptr); }

#pragma endregion

#pragma region Floor -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat floorCompiletimeImpl(TFloat x) {
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
    IS_CONST_EVALUATED { return floorCompiletimeImpl(x); }
    return std::floor(x);
}

} // detail

constexpr f32 floor(f32 x) { return detail::floor(x); }
constexpr f64 floor(f64 x) { return detail::floor(x); }

#pragma endregion

#pragma region Ceil --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat ceilCompiletimeImpl(TFloat x) {
    if (core::abs(x) == TFloat(0)) return x;
    if (core::isinf(x))            return x;
    if (core::isnan(x))            return x;

    TFloat res = core::detail::floorCompiletimeImpl(x);
    return res == x ? res : res + TFloat(1);
}

template <typename TFloat>
constexpr TFloat ceil(TFloat x) {
    IS_CONST_EVALUATED { return ceilCompiletimeImpl(x); }
    return std::ceil(x);
}

} // namespace detail

constexpr f32 ceil(f32 x) { return detail::ceil(x); }
constexpr f64 ceil(f64 x) { return detail::ceil(x); }

#pragma endregion

#pragma region Trunc -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat truncCompiletimeImpl(TFloat x) {
    if (core::abs(x) == TFloat(0)) return x;
    if (core::isinf(x))            return x;
    if (core::isnan(x))            return x;

    return (x > 0) ? floorCompiletimeImpl(x) : ceilCompiletimeImpl(x);
}

template <typename TFloat>
constexpr TFloat trunc(TFloat x) {
    IS_CONST_EVALUATED { return truncCompiletimeImpl(x); }
    return std::trunc(x);
}

} // namespace detail

constexpr f32 trunc(f32 x) { return detail::trunc(x); }
constexpr f64 trunc(f64 x) { return detail::trunc(x); }

#pragma endregion

#pragma region Round -------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat roundCompiletimeImpl(TFloat x) {
    if (detail::isnanCompiletimeImpl(x)) return x;
    if (detail::isinfCompiletimeImpl(x)) return x;
    if (core::abs(x) == TFloat(0))       return x;

    TFloat iptr = 0;
    const TFloat modx = core::detail::modfCompiletimeImpl(x, &iptr);
    constexpr TFloat half = TFloat(1)/2;

    if (modx >= half && iptr >= 0) return iptr + 1;
    if (core::abs(modx) >= half && iptr <= 0) return iptr - 1;
    return iptr;
}

template <typename TFloat>
constexpr TFloat round(TFloat x) {
    IS_CONST_EVALUATED { return roundCompiletimeImpl(x); }
    return std::round(x);
}

} // namespace detail

constexpr f32 round(f32 x) { return detail::round(x); }
constexpr f64 round(f64 x) { return detail::round(x); }

namespace detail {

template <typename TFloat>
constexpr TFloat roundTo(TFloat n, u32 to) {
    static_assert(core::is_float_v<TFloat>, "Invalid TFloat argument.");
    return TFloat(round(n * TFloat(10*to))) / TFloat(10*to);
}

} // namespace detail

constexpr f32 roundTo(f32 n, u32 to) { return detail::roundTo(n, to); }
constexpr f64 roundTo(f64 n, u32 to) { return detail::roundTo(n, to); }

#pragma endregion

#pragma region Sqrt --------------------------------------------------------------------------------------------------

namespace detail {

template <typename TFloat>
constexpr TFloat sqrtCompiletimeImpl(TFloat x) {
    // Newton's algorithm
    if (x < TFloat(0)) {
        if constexpr (sizeof(TFloat) == 8) {
            return core::quietNaNF64();
        }
        else {
            return core::quietNaNF32();
        }
    }
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
    IS_CONST_EVALUATED { return sqrtCompiletimeImpl(x); }
    return std::sqrt(x);
}

} // namespace detail


constexpr f32 sqrt(f32 x) { return detail::sqrt(x); }
constexpr f64 sqrt(f64 x) { return detail::sqrt(x); }

#pragma endregion

#pragma region Pow --------------------------------------------------------------------------------------------------

// NOTE:
// A constexpr version of pow is possible, but it will have significant differences with std::pow on different standard
// libraries, unless it's implemented very carefully. That sounds annoying to do so I'm not going to bother.

inline f32 pow(f32 x, f32 exp) { return ::powf(x, exp); }
inline f64 pow(f64 x, f64 exp) { return ::pow(x, exp); }

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

    // Decrement by 1 in case the number is already a power of 2 ensuring it won't go up to the next power.
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

#pragma region Pi

constexpr f64 piF64() { return 3.14159265358979323846; }
constexpr f32 piF32() { return 3.14159265358979323846f; }
constexpr f64 pi()    { return piF64(); }

#pragma endregion

#pragma region Radians/Degrees ----------------------------------------------------------------------------------------

struct radians {
    f32 value;
    radians() = default;
    radians(const radians&) = default;
    radians(radians&&) = default;
    constexpr explicit radians(f32 v) : value(v) {}
    constexpr explicit operator f32() const { return value; }
};

// Should be obvious to the compiler that this struct is a very simple wrapper over a single f32.
static_assert(std::is_trivial_v<radians>);
static_assert(sizeof(radians) == sizeof(f32));

constexpr radians degToRad(f32 n) {
    return radians(n * (core::piF32() / 180.0f));
}

constexpr f32 radToDeg(const radians& n) {
    return n.value * (180.0f / core::piF32());
}

#pragma endregion

#pragma region Min/Max/Clamp -----------------------------------------------------------------------------------------

template <typename T>
constexpr T core_max(const T& a, const T& b) { // apparently Windows defines a max macro, so this is core_max instead of max.
    if (a < b) return b;
    return a;
}

template <typename T>
constexpr T core_min(const T& a, const T& b) { // naming this core_min for consistency with core_max.
    if (a > b) return b;
    return a;
}

template <typename T>
constexpr tuple<T, T> minmax(const T& a, const T& b) {
    return core::createTuple(core::core_min(a, b), core::core_max(a, b));
}

template <typename T>
constexpr T clamp(const T& value, const T& min, const T& max) {
    return core::core_max(min, core::core_min(max, value));
}

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
 *        Passing NaN or infinity to any of the arguments is undefined behavior.
*/
constexpr bool safeEq(f32 a, f32 b, f32 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Safely very basic epsilon comparison.
 *        Passing NaN or infinity to any of the arguments is undefined behavior.
*/
constexpr bool safeEq(f64 a, f64 b, f64 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    return core::abs(a - b) < epsilon;
}

/**
 * @brief Compares two floating point numbers for being nearly equal, depending on the given epsilon.
 *        This is much more precise than the safeEq function, but also significantly slower.
 *        Passing NaN or infinity to any of the arguments is undefined behavior.
*/
constexpr bool nearlyEq(f32 a, f32 b, f32 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    if (a == b) return true;
    f32 absA = core::abs(a);
    f32 absB = core::abs(b);
    f32 diff = core::abs(absA - absB);
    if (a == 0 || b == 0 || (absA + absB < core::limitMin<f32>())) {
        return diff < (epsilon * core::limitMin<f32>());
    }
    return diff / core::core_min((absA + absB), core::limitMax<f32>()) < epsilon;
}

/**
 * @brief Compares two floating point numbers for being nearly equal, depending on the given epsilon.
 *        This is much more precise than the safeEq function, but also significantly slower.
 *        Passing NaN or infinity to any of the arguments is undefined behavior.
*/
constexpr bool nearlyEq(f64 a, f64 b, f64 epsilon) {
    Assert(a == a && b == b && epsilon == epsilon, "NaN is not supported.");
    if (a == b) return true;
    f64 absA = core::abs(a);
    f64 absB = core::abs(b);
    f64 diff = core::abs(absA - absB);
    if (a == 0 || b == 0 || (absA + absB < core::limitMin<f64>())) {
        return diff < (epsilon * core::limitMin<f64>());
    }
    return diff / core::core_min((absA + absB), core::limitMax<f64>()) < epsilon;
}

#pragma endregion

#pragma region Safe basic math

constexpr bool safeAdd(i8 a, i8 b, i8& out)    { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(u8 a, u8 b, u8& out)    { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(i16 a, i16 b, i16& out) { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(u16 a, u16 b, u16& out) { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(i32 a, i32 b, i32& out) { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(u32 a, u32 b, u32& out) { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(i64 a, i64 b, i64& out) { return intrin_safeAdd(a, b, out); }
constexpr bool safeAdd(u64 a, u64 b, u64& out) { return intrin_safeAdd(a, b, out); }

constexpr bool safeSub(i8 a, i8 b, i8& out)    { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(u8 a, u8 b, u8& out)    { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(i16 a, i16 b, i16& out) { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(u16 a, u16 b, u16& out) { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(i32 a, i32 b, i32& out) { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(u32 a, u32 b, u32& out) { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(i64 a, i64 b, i64& out) { return intrin_safeSub(a, b, out); }
constexpr bool safeSub(u64 a, u64 b, u64& out) { return intrin_safeSub(a, b, out); }

constexpr bool safeMul(i8 a, i8 b, i8& out)    { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(u8 a, u8 b, u8& out)    { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(i16 a, i16 b, i16& out) { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(u16 a, u16 b, u16& out) { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(i32 a, i32 b, i32& out) { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(u32 a, u32 b, u32& out) { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(i64 a, i64 b, i64& out) { return intrin_safeMul(a, b, out); }
constexpr bool safeMul(u64 a, u64 b, u64& out) { return intrin_safeMul(a, b, out); }

namespace detail {

template <typename TInt>
constexpr bool safeDiv(TInt a, TInt b, TInt& out) {
    static_assert(std::is_integral_v<TInt>, "Safe division works for integral types only.");

    // Handle zero values separately
    if (b == 0) {
        return false; // Division by zero is not allowed
    }

    if constexpr (std::is_signed_v<TInt>) {
        // Handle special overflow case: dividing the minimum value by -1
        if (a == core::limitMin<TInt>() && b == -1) {
            return false; // This would cause overflow
        }
    }

    out = a / b;
    return true;
}

} // namespace detail

constexpr bool safeDiv(i8 a, i8 b, i8& out)    { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(u8 a, u8 b, u8& out)    { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(i16 a, i16 b, i16& out) { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(u16 a, u16 b, u16& out) { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(i32 a, i32 b, i32& out) { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(u32 a, u32 b, u32& out) { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(i64 a, i64 b, i64& out) { return detail::safeDiv(a, b, out); }
constexpr bool safeDiv(u64 a, u64 b, u64& out) { return detail::safeDiv(a, b, out); }

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
