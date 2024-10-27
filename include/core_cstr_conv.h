#pragma once

#include <core_bits.h>
#include <core_cstr.h>
#include <core_expected.h>
#include <core_ints.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

// TODO2: [PERFORMACE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

enum struct ParseError : u8 {
    None,
    InputEmpty,
    InputHasMultipleDots,
    InputHasInvalidSymbol,
    InputNumberTooLarge, // will overflow return type
};
constexpr const char* parseErrorToCstr(ParseError err) {
    switch (err) {
        case ParseError::InputEmpty:             return "Input is empty";
        case ParseError::InputHasMultipleDots:   return "Input more than one dot";
        case ParseError::InputHasInvalidSymbol:  return "Input contains an invalid symbol";
        case ParseError::InputNumberTooLarge:    return "Input number is too large to fit in return type";
        case ParseError::None: break;
    }
    return "Unknown";
}

constexpr u32  intToCstr(u32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(u64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr void intToHex(u8 v, char* out, u64 hexLen = (sizeof(u8) << 1));
constexpr void intToHex(u16 v, char* out, u64 hexLen = (sizeof(u16) << 1));
constexpr void intToHex(u32 v, char* out, u64 hexLen = (sizeof(u32) << 1));
constexpr void intToHex(u64 v, char* out, u64 hexLen = (sizeof(u64) << 1));
constexpr void intToHex(i8 v, char* out, u64 hexLen = (sizeof(i8) << 1));
constexpr void intToHex(i16 v, char* out, u64 hexLen = (sizeof(i16) << 1));
constexpr void intToHex(i32 v, char* out, u64 hexLen = (sizeof(i32) << 1));
constexpr void intToHex(i64 v, char* out, u64 hexLen = (sizeof(i64) << 1));

// FIXME: Replace these when the ryu implementation is ready ---
                           constexpr u32        floatToCstr(f32 n, char* out, addr_size outMax, u32 precision = 6);
                           constexpr u32        floatToCstr(f64 n, char* out, addr_size outMax, u32 precision = 6);
// FIXME: TO HERE ---

template <typename TInt> constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen);
template <typename TFloat> constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen);

namespace detail {

template<typename TInt>
constexpr u32 intToCstr(TInt n, char* out, addr_size outMax, u32 digits) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    Assert(out);

    u32 idx = 0;

    auto safeAppend = [&idx, out, outMax](char c) {
        Assert(addr_size(idx) < outMax);
        out[idx++] = c;
    };

    if constexpr (core::is_signed_v<TInt>) {
        if (n < 0) {
            safeAppend('-');
            n = -n;
        }
    }

    i32 dc = (digits == 0) ? i32(digitCount(n)) : i32(digits);
    for (i32 i = dc - 1; i >= 0; i--) {
        // There is a lot of believe in all this static casting, but it 'should not' be dangerous.
        TInt curr = static_cast<TInt>((n / static_cast<TInt>(pow10(u32(i))))) % 10;
        safeAppend(digitToChar(curr));
        dc--;
    }

    return idx;
}

} // detail namespace

constexpr u32 intToCstr(u32 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(u64 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(i32 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(i64 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }

namespace detail {

template <typename TFloat>
constexpr u32 floatToCstr(TFloat n, char* out, addr_size outMax, u32 precision) {
    static_assert(core::is_float_v<TFloat>, "TFloat must be a floating point type.");
    Assert(out);

    if constexpr (sizeof(TFloat) == 4) {
        Assert(precision < 8, "Precision must be less than 8 for float.");
    }
    else {
        Assert(precision < 16, "Precision must be less than 16 for double.");
    }

    u32 idx = 0;

    const auto safeAppend = [&idx, out, outMax](char c) {
        Assert(addr_size(idx) < outMax);
        out[idx++] = c;
    };

    if (core::isnan(n)) {
        safeAppend('n');
        safeAppend('a');
        safeAppend('n');
        return idx;
    }

    if (core::isinf(n)) {
        if (n < 0) safeAppend('-');
        safeAppend('i');
        safeAppend('n');
        safeAppend('f');
        return idx;
    }

    if (n < 0) {
        safeAppend('-');
        n = -n;
    }

    i64 intPart = i64(n);
    u64 fracMultiplier =  core::pow10(precision);
    TFloat fracPartFloat = (n - TFloat(intPart)) * TFloat(fracMultiplier);

    // Banker's rounding:
    i64 fracPart;
    TFloat roundPart = fracPartFloat - TFloat(i64(fracPartFloat));
    if (roundPart > TFloat(0.5)) {
        fracPart = i64(fracPartFloat) + 1;
    }
    else if (roundPart < TFloat(0.5)) {
        fracPart = i64(fracPartFloat);
    }
    else { // roundPart == 0.5, tie breaker.
        fracPart = i64(fracPartFloat);
        if (fracPart & 1) {
            fracPart++;
        }
    }

    // Handle overflow from rounding.
    if (fracPart >= i64(fracMultiplier)) {
        intPart++;
        fracPart = 0;
    }

    // Convert the integer part.
    idx += detail::intToCstr(intPart, out + idx, outMax, 0);
    if (precision > 0) {
        safeAppend('.');

        // Prepend zeroes if the fractional part is less than the precision.
        u32 zeroesToPrepend = precision - core::digitCount(fracPart);
        for (u32 i = 0; i < zeroesToPrepend; i++) {
            safeAppend('0');
        }

        // Convert the fractional part.
        u32 prev = idx;
        idx += detail::intToCstr(fracPart, out + idx, outMax, 0);
        u32 currentPrecision = (idx - prev) + zeroesToPrepend;

        // Add trailing zeroes if the precision is not reached.
        for (u32 i = currentPrecision; i < precision; i++) {
            safeAppend('0');
        }
    }

    return idx;
}

} // detail

constexpr u32 floatToCstr(f32 n, char* out, addr_size outMax, u32 precision) { return detail::floatToCstr(n, out, outMax, precision); }
constexpr u32 floatToCstr(f64 n, char* out, addr_size outMax, u32 precision) { return detail::floatToCstr(n, out, outMax, precision); }

// This function does not handle TInt overflows!
template <typename TInt>
constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");

    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    u32 i = 0;
    bool neg = false;
    if constexpr (core::is_signed_v<TInt>) {
        neg = s[i] == '-';
        if (neg) i++;
    }

    TInt res = 0;
    while (i < slen) {
        char curr = s[i];
        if (!isDigit(curr)) {
            return core::unexpected(ParseError::InputHasInvalidSymbol);
        }

        TInt next = static_cast<TInt>(res * 10) + core::toDigit<TInt>(curr);
        if (next < res) {
            return core::unexpected(ParseError::InputNumberTooLarge);
        }
        res = next;
        i++;
    }

    if constexpr (core::is_signed_v<TInt>) {
        if (neg) res = -res;
    }

    return res;
}

#pragma region Cptr to float

// FIXME: Add a comment giving credit to the creator of the algorithm.

namespace detail {

constexpr u64 FLOAT_POW5_INV_SPLIT[55] = {
    576460752303423489u, 461168601842738791u, 368934881474191033u, 295147905179352826u,
    472236648286964522u, 377789318629571618u, 302231454903657294u, 483570327845851670u,
    386856262276681336u, 309485009821345069u, 495176015714152110u, 396140812571321688u,
    316912650057057351u, 507060240091291761u, 405648192073033409u, 324518553658426727u,
    519229685853482763u, 415383748682786211u, 332306998946228969u, 531691198313966350u,
    425352958651173080u, 340282366920938464u, 544451787073501542u, 435561429658801234u,
    348449143727040987u, 557518629963265579u, 446014903970612463u, 356811923176489971u,
    570899077082383953u, 456719261665907162u, 365375409332725730u, 292300327466180584u,
    467680523945888934u, 374144419156711148u, 299315535325368918u, 478904856520590269u,
    383123885216472215u, 306499108173177772u, 490398573077084435u, 392318858461667548u,
    313855086769334039u, 502168138830934462u, 401734511064747569u, 321387608851798056u,
    514220174162876889u, 411376139330301511u, 329100911464241209u, 526561458342785934u,
    421249166674228747u, 336999333339382998u, 539198933343012796u, 431359146674410237u,
    345087317339528190u, 552139707743245103u, 441711766194596083u
};

constexpr inline u32 mulShift32(u32 m, u64 factor, i32 shift) {
    Assert(shift > 32);

    // The casts here help MSVC to avoid calls to the __allmul library function.
    u32 factorLo = u32(factor);
    u32 factorHi = u32(factor >> 32);
    u64 bits0 = u64(m) * u64(factorLo);
    u64 bits1 = u64(m) * u64(factorHi);

    u64 sum = (bits0 >> 32) + bits1;
    u64 shiftedSum = sum >> (shift - 32);

    Assert(shiftedSum <= core::limitMax<u32>());

    return u32(shiftedSum);
}

constexpr inline i32 mulPow5InvDivPow2(u32 m, u32 i, u32 j) {
    return mulShift32(m, FLOAT_POW5_INV_SPLIT[i], j);
}

constexpr u32 floorLog2(u32 x) {
    return 31 - core::intrin_countLeadingZeros(x);
}

// Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
constexpr inline i32 log2pow5(i32 e) {
    // This approximation works up to the point that the multiplication overflows at e = 3529.
    // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
    // than 2^9297.
    Assert(e >= 0);
    Assert(e <= 3528);
    return i32((u32(e) * 1217359) >> 19);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
constexpr inline i32 ceilLog2pow5(i32 e) {
    return log2pow5(e) + 1;
}

// Returns true if value is divisible by 2^p.
constexpr inline bool multipleOfPowerOf2_32(u32 value, u32 p) {
    return (value & ((1u << p) - 1)) == 0;
}

constexpr inline u32 pow5factor_32(u32 value) {
    u32 count = 0;
    for (;;) {
        Assert(value != 0);
        u32 q = value / 5;
        u32 r = value % 5;
        if (r != 0) break;
        value = q;
        ++count;
    }
    return count;
}

constexpr inline bool multipleOfPowerOf5_32(u32 value, u32 p) {
    return pow5factor_32(value) >= p;
}

} // detail

// This function does not handle TFloat overflows!
template <typename TFloat>
constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen) {
    static_assert(std::is_floating_point_v<TFloat>);

    static_assert(sizeof(TFloat) != 8, "FIXME: not implemented yet");

    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    using detail::floorLog2;
    using detail::ceilLog2pow5;
    using detail::mulPow5InvDivPow2;
    using detail::multipleOfPowerOf2_32;
    using detail::multipleOfPowerOf5_32;

    constexpr i32 MAX_MANTISA_DIGITS = i32(core::maxMantisaDigitsBase10<TFloat>());
    constexpr i32 MANTISSA_BITS = i32(core::mantisaBits<TFloat>());
    constexpr i32 EXPONENT_BITS = i32(core::exponentBits<TFloat>());

    constexpr u32 FLOAT_POW5_INV_BITCOUNT = 59; // FIXME: Value is invalid for f64
    constexpr u32 FLOAT_EXPONENT_BIAS = 127; // FIXME: Value is invalid for f64

    i32 mantissaDigits = 0;
    u32 mantissa = 0;
    i32 exponent = 0;
    u32 dotIndex = slen;
    bool isNegative = false;

    u32 i = 0;
    if (s[i] == '-') {
        isNegative = true;
        i++;
    }

    // check for special strings
    {
        if (slen - i == 3) {
            if ((s[0] == 'n' || s[0] == 'N') &&
                (s[1] == 'a' || s[1] == 'A') &&
                (s[2] == 'n' || s[2] == 'N')
            ) {
                return core::signalingNaN<TFloat>();
            }
            if ((s[0] == 'i' || s[0] == 'i') &&
                (s[1] == 'n' || s[1] == 'n') &&
                (s[2] == 'f' || s[2] == 'f')
            ) {
                return isNegative ? -core::infinity<TFloat>() : core::infinity<TFloat>();
            }
        }
    }

    for (; i < slen; i++) {
        char c = s[i];
        if (c == '.') {
            if (dotIndex != slen) {
                return core::unexpected(ParseError::InputHasMultipleDots);
            }
            dotIndex = i;
        }
        else {
            if (!core::isDigit(c)) return core::unexpected(ParseError::InputHasInvalidSymbol);
            if (mantissaDigits >= MAX_MANTISA_DIGITS) return core::unexpected(ParseError::InputNumberTooLarge);
            mantissa = 10 * mantissa + core::toDigit<decltype(mantissa)>(c);
            if (mantissa != 0) mantissaDigits++;
        }
    }

    if (mantissa == 0) {
        return isNegative ? -0.0f : 0.0f;
    }

    exponent -= slen != dotIndex ? slen - i32(dotIndex) - 1 : 0;

    // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion was exact.
    i32 e2, m2;
    bool trailingZeros;
    {
        e2 = floorLog2(mantissa) + exponent - ceilLog2pow5(-exponent) - (MANTISSA_BITS + 1);

        // We now compute [m * 10^e / 2^e2] = [m / (5^(-e) 2^(e2-e))].
        i32 j = e2 - exponent + ceilLog2pow5(-exponent) - 1 + FLOAT_POW5_INV_BITCOUNT;
        m2 = mulPow5InvDivPow2(mantissa, -exponent, j);

        // We also compute if the result is exact, i.e.,
        //   [m / (5^(-e) 2^(e2-e))] == m / (5^(-e) 2^(e2-e))
        //
        // If e2-e >= 0, we need to check whether (5^(-e) 2^(e2-e)) divides m, which is the
        // case iff pow5(m) >= -e AND pow2(m) >= e2-e.
        //
        // If e2-e < 0, we have actually computed [m * 2^(e e2) / 5^(-e)] above,
        // and we need to check whether 5^(-e) divides (m * 2^(e-e2)), which is the case iff
        // pow5(m * 2^(e-e2)) = pow5(m) >= -e.
        trailingZeros = (e2 < exponent || (e2 - exponent < 32 && multipleOfPowerOf2_32(mantissa, e2 - exponent)))
                         && multipleOfPowerOf5_32(mantissa, -exponent);
    }

    // Compute the final IEEE exponent.
    u32 ieee_e2 = core::core_max(0u, u32(e2) + FLOAT_EXPONENT_BIAS + floorLog2(m2));
    if (ieee_e2 > 0xfe) {
        // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
        u32 ieee = ((u32(isNegative)) << u32(EXPONENT_BITS + MANTISSA_BITS)) | (0xffu << MANTISSA_BITS);
        TFloat ret = core::bitCast<TFloat>(ieee);
        return ret;
    }

    // We need to figure out how much we need to shift m2. The tricky part is that we need to take the final IEEE
    // exponent into account, so we need to reverse the bias and also special-case the value 0.
    i32 shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - FLOAT_EXPONENT_BIAS - MANTISSA_BITS;
    Assert(shift >= 0);

    // We need to round up if the exact value is more than 0.5 above the value we computed. That's equivalent to
    // checking if the last removed bit was 1 and either the value was not just trailing zeros or the result would
    // otherwise be odd.
    //
    // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
    trailingZeros &= (m2 & ((1u << (shift - 1)) - 1)) == 0;
    u32 lastRemovedBit = (m2 >> (shift - 1)) & 1;
    bool roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

    u32 ieee_m2 = (m2 >> shift) + roundUp;
    Assert(ieee_m2 <= (1u << (MANTISSA_BITS + 1)));
    ieee_m2 &= (1u << MANTISSA_BITS) - 1;
    if (ieee_m2 == 0 && roundUp) {
        // Rounding up may overflow the mantissa.
        // In this case we move a trailing zero of the mantissa into the exponent.
        // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
        ieee_e2++;
    }

    u32 ieee = (((u32(isNegative) << EXPONENT_BITS) | u32(ieee_e2)) << MANTISSA_BITS) | ieee_m2;
    TFloat ret = core::bitCast<TFloat>(ieee);
    return ret;
}

#pragma endregion

namespace detail {

static constexpr const char* hexDigits = "0123456789ABCDEF";

// The out argument must have enough space to hold the result!
template <typename TInt>
constexpr void intToHex(TInt v, char* out, u64 hexLen) {
    for (size_t i = 0, j = (hexLen - 1) * 4; i < hexLen; i++, j-=4) {
        out[i] = detail::hexDigits[(v >> j) & 0x0f];
    }
}

} // namespace detail

constexpr void intToHex(u8 v, char* out, u64 hexLen)  { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u16 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u32 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u64 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i8 v, char* out, u64 hexLen)  { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i16 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i32 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i64 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }

} // namespace core
