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
    OutputBufferTooSmall, // will overflow provided buffer
};

constexpr const char* parseErrorToCstr(ParseError err) {
    switch (err) {
        case ParseError::InputEmpty:             return "Input is empty";
        case ParseError::InputHasMultipleDots:   return "Input has more than one dot";
        case ParseError::InputHasInvalidSymbol:  return "Input contains an invalid symbol";
        case ParseError::InputNumberTooLarge:    return "Input number is too large to fit in return type";
        case ParseError::OutputBufferTooSmall:   return "Output buffer is too small to fit the parsed number";
        case ParseError::None: break;
    }
    return "Unknown";
}

template <typename TInt> constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen);

constexpr u32  intToCstr(u32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(u64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr void intToHex(u8 v, char* out, u64 hexLen = (sizeof(u8) << 1)); // TODO: alter the to hex api to match the behaviour of toCstr
constexpr void intToHex(u16 v, char* out, u64 hexLen = (sizeof(u16) << 1));
constexpr void intToHex(u32 v, char* out, u64 hexLen = (sizeof(u32) << 1));
constexpr void intToHex(u64 v, char* out, u64 hexLen = (sizeof(u64) << 1));
constexpr void intToHex(i8 v, char* out, u64 hexLen = (sizeof(i8) << 1));
constexpr void intToHex(i16 v, char* out, u64 hexLen = (sizeof(i16) << 1));
constexpr void intToHex(i32 v, char* out, u64 hexLen = (sizeof(i32) << 1));
constexpr void intToHex(i64 v, char* out, u64 hexLen = (sizeof(i64) << 1));

template <typename TFloat> constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen);
                           constexpr u32                                floatToCstr(f32 n, char* out, u32 olen);
                           constexpr u32                                floatToCstr(f64 n, char* out, u32 olen);
                           constexpr u32                                floatToFixedCstr(f64 n, u32 precision, char* out, u32 olen);

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
        TInt curr = TInt(n / TInt(pow10(u32(i)))) % 10;
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

// This function does not handle TInt overflows!
template <typename TInt>
constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");

    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    u32 i = 0;
    [[maybe_unused]] bool neg = false;
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

        TInt next = TInt(res * 10) + core::toDigit<TInt>(curr);
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

namespace detail {

// FIXME: Overflow writes to output buffers is not handled at all?

static constexpr const char* hexDigits = "0123456789ABCDEF";

// The out argument must have enough space to hold the result!
template <typename TInt>
constexpr void intToHex(TInt v, char* out, u64 hexLen) {
    for (addr_size i = 0, j = (hexLen - 1) * 4; i < hexLen; i++, j-=4) {
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

#pragma region Cptr to float

namespace detail {

constexpr char DIGIT_TABLE[200] = {
    '0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
    '1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
    '2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
    '3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
    '4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
    '5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
    '6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
    '7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
    '8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
    '9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
};

constexpr inline u64 umul128(u64 a,u64 b, u64* productHi) {
    // The casts here help MSVC to avoid calls to the __allmul library function.
    u32 aLo = u32(a);
    u32 aHi = u32((a >> 32));
    u32 bLo = u32(b);
    u32 bHi = u32((b >> 32));

    u64 b00 = u64(aLo) * u64(bLo);
    u64 b01 = u64(aLo) * u64(bHi);
    u64 b10 = u64(aHi) * u64(bLo);
    u64 b11 = u64(aHi) * u64(bHi);

    u32 b00Lo = u32(b00);
    u32 b00Hi = u32(b00 >> 32);

    u64 mid1 = b10 + b00Hi;
    u32 mid1Lo = u32(mid1);
    u32 mid1Hi = u32(mid1 >> 32);

    u64 mid2 = b01 + mid1Lo;
    u32 mid2Lo = u32(mid2);
    u32 mid2Hi = u32(mid2 >> 32);

    u64 pHi = b11 + mid1Hi + mid2Hi;
    u64 pLo = (u64(mid2Lo) << 32) | u64(b00Lo);

    *productHi = pHi;
    return pLo;
}

constexpr inline u64 shiftright128(u64 lo, u64 hi, u32 dist) {
    Assert(dist < 64);
    Assert(dist > 0);
    return (hi << (64 - dist)) | (lo >> dist);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
constexpr inline i32 pow5bits(i32 e) {
    // This approximation works up to the point that the multiplication overflows at e = 3529.
    // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
    // than 2^9297.
    Assert(e >= 0);
    Assert(e <= 3528);
    return i32((u32(e) * 1217359) >> 19) + 1;
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

// Returns floor(log_10(2^e)); requires 0 <= e <= 1650.
constexpr inline u32 log10pow2(i32 e) {
    // The first value this approximation fails for is 2^1651 which is just greater than 10^297.
    Assert(e >= 0);
    Assert(e <= 1650);
    return (u32(e) * 78913) >> 18;
}

// Returns floor(log_10(5^e)); requires 0 <= e <= 2620.
constexpr inline u32 log10pow5(i32 e) {
    // The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
    Assert(e >= 0);
    Assert(e <= 2620);
    return (u32(e) * 732923) >> 20;
}

constexpr inline u32 mod1e9(u64 x) {
  return u32(x - 1000000000 * (x / 1000000000));
}

constexpr inline u32 decimalLength9(u32 v) {
    Assert(v < 1000000000);
    if (v >= 100000000) { return 9; }
    if (v >= 10000000) { return 8; }
    if (v >= 1000000) { return 7; }
    if (v >= 100000) { return 6; }
    if (v >= 10000) { return 5; }
    if (v >= 1000) { return 4; }
    if (v >= 100) { return 3; }
    if (v >= 10) { return 2; }
    return 1;
}

constexpr inline u32 decimalLength17(u64 v) {
    // This is slightly faster than a loop.
    // The average output length is 16.38 digits, so we check high-to-low.
    // Function precondition: v is not an 18, 19, or 20-digit number.
    // (17 digits are sufficient for round-tripping.)
    Assert(v < 100000000000000000L);
    if (v >= 10000000000000000L) { return 17; }
    if (v >= 1000000000000000L) { return 16; }
    if (v >= 100000000000000L) { return 15; }
    if (v >= 10000000000000L) { return 14; }
    if (v >= 1000000000000L) { return 13; }
    if (v >= 100000000000L) { return 12; }
    if (v >= 10000000000L) { return 11; }
    if (v >= 1000000000L) { return 10; }
    if (v >= 100000000L) { return 9; }
    if (v >= 10000000L) { return 8; }
    if (v >= 1000000L) { return 7; }
    if (v >= 100000L) { return 6; }
    if (v >= 10000L) { return 5; }
    if (v >= 1000L) { return 4; }
    if (v >= 100L) { return 3; }
    if (v >= 10L) { return 2; }
    return 1;
}

constexpr inline i32 copySpecialStr(bool sign, bool exponent, bool mantissa, char* out, [[maybe_unused]] u32 olen, bool exponentFormant = true) {
    Assert(olen > u32(sign) + 3, "provider buffer is short");

    if (mantissa) {
        core::memcopy(out, "NaN", 3);
        return 3;
    }
    if (sign) {
        out[0] = '-';
    }
    if (exponent) {
        core::memcopy(out + sign, "Inf", 3);
        return i32(sign) + 3;
    }
    if (exponentFormant) {
        core::memcopy(out + sign, "0E0", 3);
        return i32(sign) + 3;
    }

    core::memcopy(out + sign, "0", 1);
    return i32(sign) + 1;
}

// Convert `digits` to decimal and write the last `count` decimal digits to result.
// If `digits` contains additional digits, then those are silently ignored.
constexpr inline void appendDigitsCutExcess(u32 count, u32 digits, char* result) {
    // Copy pairs of digits from DIGIT_TABLE.
    u32 i = 0;
    for (; i < count - 1; i += 2) {
        u32 c = (digits % 100) << 1;
        digits /= 100;
        core::memcopy(result + count - i - 2, DIGIT_TABLE + c, 2);
    }
    // Generate the last digit if count is odd.
    if (i < count) {
        char c =  core::digitToChar(digits);
        result[count - i - 1] = c;
    }
}


// Convert `digits` to decimal and write the last 9 decimal digits to result.
// If `digits` contains additional digits, then those are silently ignored.
constexpr inline void appendNineDigits(u32 digits, char* out) {
    if (digits == 0) {
        core::memset(out, '0', 9);
        return;
    }

    for (u32 i = 0; i < 5; i += 4) {
        u32 c = digits % 10000;
        digits /= 10000;
        const u32 c0 = (c % 100) << 1;
        const u32 c1 = (c / 100) << 1;
        core::memcopy(out + 7 - i, DIGIT_TABLE + c0, 2);
        core::memcopy(out + 5 - i, DIGIT_TABLE + c1, 2);
    }
    out[0] = core::digitToChar(char('0' + digits));
}

template<typename TFloat> struct FloatTraits;

template<>
struct FloatTraits<f32> {
    using UInt = u32;

    struct FloatDecimal {
        u32 mantissa;
        i32 exponent;
    };

    static constexpr i32 MAX_MANTISSA_DIGITS = core::maxMantissaDigitsBase10<f32>();
    static constexpr i32 MANTISSA_BITS = core::mantissaBits<f32>();
    static constexpr i32 EXPONENT_BITS = core::exponentBits<f32>();
    static constexpr u32 EXPONENT_BIAS = core::exponentBias<f32>();
    static constexpr u32 POW5_BITCOUNT = 61;
    static constexpr u32 POW5_INV_BITCOUNT = 59;
    static constexpr i32 MIN_EXPONENT_AFTER_WHICH_ROUND_TO_ZERO = -46;
    static constexpr i32 MAX_EXPONENT_AFTER_WHICH_ROUND_TO_INF = 40;
    static constexpr u32 MANTISSA_BITS_MASK = 0xffu;

    static constexpr u32 POW5_INV_SPLIT_SIZE = 55;
    static constexpr u64 POW5_INV_SPLIT[POW5_INV_SPLIT_SIZE] = {
        576460752303423489u,  461168601842738791u,  368934881474191033u,  295147905179352826u,
        472236648286964522u,  377789318629571618u,  302231454903657294u,  483570327845851670u,
        386856262276681336u,  309485009821345069u,  495176015714152110u,  396140812571321688u,
        316912650057057351u,  507060240091291761u,  405648192073033409u,  324518553658426727u,
        519229685853482763u,  415383748682786211u,  332306998946228969u,  531691198313966350u,
        425352958651173080u,  340282366920938464u,  544451787073501542u,  435561429658801234u,
        348449143727040987u,  557518629963265579u,  446014903970612463u,  356811923176489971u,
        570899077082383953u,  456719261665907162u,  365375409332725730u,  292300327466180584u,
        467680523945888934u,  374144419156711148u,  299315535325368918u,  478904856520590269u,
        383123885216472215u,  306499108173177772u,  490398573077084435u,  392318858461667548u,
        313855086769334039u,  502168138830934462u,  401734511064747569u,  321387608851798056u,
        514220174162876889u,  411376139330301511u,  329100911464241209u,  526561458342785934u,
        421249166674228747u,  336999333339382998u,  539198933343012796u,  431359146674410237u,
        345087317339528190u,  552139707743245103u,  441711766194596083u
    };

    static constexpr u32 POW5_SPLIT_SIZE = 47;
    static constexpr u64 POW5_SPLIT[POW5_SPLIT_SIZE] = {
        1152921504606846976u, 1441151880758558720u, 1801439850948198400u, 2251799813685248000u,
        1407374883553280000u, 1759218604441600000u, 2199023255552000000u, 1374389534720000000u,
        1717986918400000000u, 2147483648000000000u, 1342177280000000000u, 1677721600000000000u,
        2097152000000000000u, 1310720000000000000u, 1638400000000000000u, 2048000000000000000u,
        1280000000000000000u, 1600000000000000000u, 2000000000000000000u, 1250000000000000000u,
        1562500000000000000u, 1953125000000000000u, 1220703125000000000u, 1525878906250000000u,
        1907348632812500000u, 1192092895507812500u, 1490116119384765625u, 1862645149230957031u,
        1164153218269348144u, 1455191522836685180u, 1818989403545856475u, 2273736754432320594u,
        1421085471520200371u, 1776356839400250464u, 2220446049250313080u, 1387778780781445675u,
        1734723475976807094u, 2168404344971008868u, 1355252715606880542u, 1694065894508600678u,
        2117582368135750847u, 1323488980084844279u, 1654361225106055349u, 2067951531382569187u,
        1292469707114105741u, 1615587133892632177u, 2019483917365790221u
    };

    static constexpr u32 floorLog2(u32 x) {
        return 31 - core::intrin_countLeadingZeros(x);
    }

    static constexpr u32 mulShift(u32 m, u64 factor, u32 shift) {
        Assert(shift > 32);

        u32 factorLo = u32(factor);
        u32 factorHi = u32(factor >> 32);
        u64 bits0 = u64(m) * u64(factorLo);
        u64 bits1 = u64(m) * u64(factorHi);

        u64 sum = (bits0 >> 32) + bits1;
        u64 shiftedSum = sum >> (shift - 32);

        Assert(shiftedSum <= core::limitMax<u32>());

        return u32(shiftedSum);
    }

    static constexpr u32 pow5Factor(u32 value) {
        u32 count = 0;
        while (value % 5 == 0) {
            value /= 5;
            count++;
        }
        return count;
    }

    static constexpr bool multipleOfPowerOf5(u32 value, u32 p) {
        return pow5Factor(value) >= p;
    }

    static constexpr bool multipleOfPowerOf2(u32 value, u32 p) {
        return (value & ((1u << p) - 1)) == 0;
    }

    static constexpr bool multipleOfPowerOf2(u64 value, u32 p) {
        Assert(value != 0);
        Assert(p < 64);
        // __builtin_ctzll doesn't appear to be faster here.
        return (value & ((1ull << p) - 1)) == 0;
    }

    static constexpr bool convertFloatToBinary(u32 mantissa, i32 exponent, u32& m2, i32& e2) {
        // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion
        // was exact (trailingZeros).
        bool trailingZeros;
        if (exponent >= 0) {
            // The length of m * 10^e in bits is:
            //   log2(mantissa * 10^exponent) = log2(mantissa) + exponent log2(10) = log2(mantissa) + exponent + exponent * log2(5)
            //
            // We want to compute the MANTISSA_BITS + 1 top-most bits (+1 for the implicit leading
            // one in IEEE format). We therefore choose a binary output exponent of
            //   log2(mantissa * 10^exponent) - (MANTISSA_BITS + 1).
            //
            // We use floor(log2(5^exponent)) so that we get at least this many bits; better to
            // have an additional bit than to not have enough bits.
            e2 = floorLog2(mantissa) + exponent + log2pow5(exponent) - (MANTISSA_BITS + 1);

            // We now compute [mantissa * 10^exponent / 2^e2] = [mantissa * 5^exponent / 2^(e2-exponent)].
            // To that end, we use the POW5_SPLIT table.
            int j = e2 - exponent - ceilLog2pow5(exponent) + POW5_BITCOUNT;
            Assert(j >= 0);

            m2 = mulShift(mantissa, POW5_SPLIT[exponent], j);

            // We also compute if the result is exact, i.e.,
            //   [mantissa * 10^exponent / 2^e2] == mantissa * 10^exponent / 2^e2.
            // This can only be the case if 2^e2 divides mantissa * 10^exponent, which in turn requires that the
            // largest power of 2 that divides mantissa + exponent is greater than e2. If e2 is less than exponent, then
            // the result must be exact. Otherwise we use the existing multipleOfPowerOf2 function.
            trailingZeros = e2 < exponent || (e2 - exponent < 32 && multipleOfPowerOf2(mantissa, e2 - exponent));
        }
        else {
            e2 = floorLog2(mantissa) + exponent - ceilLog2pow5(-exponent) - (MANTISSA_BITS + 1);

            // We now compute [mantissa * 10^exponent / 2^e2] = [mantissa / (5^(-exponent) 2^(e2-exponent))].
            int j = e2 - exponent + ceilLog2pow5(-exponent) - 1 + POW5_INV_BITCOUNT;
            m2 = mulShift(mantissa, POW5_INV_SPLIT[-exponent], j);

            // We also compute if the result is exact, i.e.,
            //   [mantissa / (5^(-exponent) 2^(e2-exponent))] == mantissa / (5^(-exponent) 2^(e2-exponent))
            //
            // If e2-exponent >= 0, we need to check whether (5^(-exponent) 2^(e2-exponent)) divides mantissa, which is the
            // case iff pow5(mantissa) >= -exponent AND pow2(mantissa) >= e2-exponent.
            //
            // If e2-exponent < 0, we have actually computed [mantissa * 2^(exponent e2) / 5^(-exponent)] above,
            // and we need to check whether 5^(-exponent) divides (mantissa * 2^(exponent-e2)), which is the case iff
            // pow5(mantissa * 2^(exponent-e2)) = pow5(mantissa) >= -exponent.
            trailingZeros = (e2 < exponent || (e2 - exponent < 32 && multipleOfPowerOf2(mantissa, e2 - exponent)))
                && multipleOfPowerOf5(mantissa, u32(-exponent));
        }

        return trailingZeros;
    }
};

template<>
struct FloatTraits<f64> {
    using UInt = u64;

    struct FloatDecimal {
        u64 mantissa;
        i32 exponent;
    };

    static constexpr i32 MAX_MANTISSA_DIGITS = core::maxMantissaDigitsBase10<f64>();
    static constexpr i32 MANTISSA_BITS = core::mantissaBits<f64>();
    static constexpr i32 EXPONENT_BITS = core::exponentBits<f64>();
    static constexpr u32 EXPONENT_BIAS = core::exponentBias<f64>();
    static constexpr u32 POW5_INV_BITCOUNT = 125;
    static constexpr u32 POW5_BITCOUNT = 125;

    static constexpr i32 MIN_EXPONENT_AFTER_WHICH_ROUND_TO_ZERO = -324;
    static constexpr i32 MAX_EXPONENT_AFTER_WHICH_ROUND_TO_INF = 310;
    static constexpr u64 MANTISSA_BITS_MASK = 0x7ffull;

    static constexpr u32 POW5_INV_TABLE_SIZE = 342;
    static constexpr u64 POW5_INV_SPLIT[POW5_INV_TABLE_SIZE][2] = {
        {                    1u, 2305843009213693952u }, { 11068046444225730970u, 1844674407370955161u },
        {  5165088340638674453u, 1475739525896764129u }, {  7821419487252849886u, 1180591620717411303u },
        {  8824922364862649494u, 1888946593147858085u }, {  7059937891890119595u, 1511157274518286468u },
        { 13026647942995916322u, 1208925819614629174u }, {  9774590264567735146u, 1934281311383406679u },
        { 11509021026396098440u, 1547425049106725343u }, { 16585914450600699399u, 1237940039285380274u },
        { 15469416676735388068u, 1980704062856608439u }, { 16064882156130220778u, 1584563250285286751u },
        {  9162556910162266299u, 1267650600228229401u }, {  7281393426775805432u, 2028240960365167042u },
        { 16893161185646375315u, 1622592768292133633u }, {  2446482504291369283u, 1298074214633706907u },
        {  7603720821608101175u, 2076918743413931051u }, {  2393627842544570617u, 1661534994731144841u },
        { 16672297533003297786u, 1329227995784915872u }, { 11918280793837635165u, 2126764793255865396u },
        {  5845275820328197809u, 1701411834604692317u }, { 15744267100488289217u, 1361129467683753853u },
        {  3054734472329800808u, 2177807148294006166u }, { 17201182836831481939u, 1742245718635204932u },
        {  6382248639981364905u, 1393796574908163946u }, {  2832900194486363201u, 2230074519853062314u },
        {  5955668970331000884u, 1784059615882449851u }, {  1075186361522890384u, 1427247692705959881u },
        { 12788344622662355584u, 2283596308329535809u }, { 13920024512871794791u, 1826877046663628647u },
        {  3757321980813615186u, 1461501637330902918u }, { 10384555214134712795u, 1169201309864722334u },
        {  5547241898389809503u, 1870722095783555735u }, {  4437793518711847602u, 1496577676626844588u },
        { 10928932444453298728u, 1197262141301475670u }, { 17486291911125277965u, 1915619426082361072u },
        {  6610335899416401726u, 1532495540865888858u }, { 12666966349016942027u, 1225996432692711086u },
        { 12888448528943286597u, 1961594292308337738u }, { 17689456452638449924u, 1569275433846670190u },
        { 14151565162110759939u, 1255420347077336152u }, {  7885109000409574610u, 2008672555323737844u },
        {  9997436015069570011u, 1606938044258990275u }, {  7997948812055656009u, 1285550435407192220u },
        { 12796718099289049614u, 2056880696651507552u }, {  2858676849947419045u, 1645504557321206042u },
        { 13354987924183666206u, 1316403645856964833u }, { 17678631863951955605u, 2106245833371143733u },
        {  3074859046935833515u, 1684996666696914987u }, { 13527933681774397782u, 1347997333357531989u },
        { 10576647446613305481u, 2156795733372051183u }, { 15840015586774465031u, 1725436586697640946u },
        {  8982663654677661702u, 1380349269358112757u }, { 18061610662226169046u, 2208558830972980411u },
        { 10759939715039024913u, 1766847064778384329u }, { 12297300586773130254u, 1413477651822707463u },
        { 15986332124095098083u, 2261564242916331941u }, {  9099716884534168143u, 1809251394333065553u },
        { 14658471137111155161u, 1447401115466452442u }, {  4348079280205103483u, 1157920892373161954u },
        { 14335624477811986218u, 1852673427797059126u }, {  7779150767507678651u, 1482138742237647301u },
        {  2533971799264232598u, 1185710993790117841u }, { 15122401323048503126u, 1897137590064188545u },
        { 12097921058438802501u, 1517710072051350836u }, {  5988988032009131678u, 1214168057641080669u },
        { 16961078480698431330u, 1942668892225729070u }, { 13568862784558745064u, 1554135113780583256u },
        {  7165741412905085728u, 1243308091024466605u }, { 11465186260648137165u, 1989292945639146568u },
        { 16550846638002330379u, 1591434356511317254u }, { 16930026125143774626u, 1273147485209053803u },
        {  4951948911778577463u, 2037035976334486086u }, {   272210314680951647u, 1629628781067588869u },
        {  3907117066486671641u, 1303703024854071095u }, {  6251387306378674625u, 2085924839766513752u },
        { 16069156289328670670u, 1668739871813211001u }, {  9165976216721026213u, 1334991897450568801u },
        {  7286864317269821294u, 2135987035920910082u }, { 16897537898041588005u, 1708789628736728065u },
        { 13518030318433270404u, 1367031702989382452u }, {  6871453250525591353u, 2187250724783011924u },
        {  9186511415162383406u, 1749800579826409539u }, { 11038557946871817048u, 1399840463861127631u },
        { 10282995085511086630u, 2239744742177804210u }, {  8226396068408869304u, 1791795793742243368u },
        { 13959814484210916090u, 1433436634993794694u }, { 11267656730511734774u, 2293498615990071511u },
        {  5324776569667477496u, 1834798892792057209u }, {  7949170070475892320u, 1467839114233645767u },
        { 17427382500606444826u, 1174271291386916613u }, {  5747719112518849781u, 1878834066219066582u },
        { 15666221734240810795u, 1503067252975253265u }, { 12532977387392648636u, 1202453802380202612u },
        {  5295368560860596524u, 1923926083808324180u }, {  4236294848688477220u, 1539140867046659344u },
        {  7078384693692692099u, 1231312693637327475u }, { 11325415509908307358u, 1970100309819723960u },
        {  9060332407926645887u, 1576080247855779168u }, { 14626963555825137356u, 1260864198284623334u },
        { 12335095245094488799u, 2017382717255397335u }, {  9868076196075591040u, 1613906173804317868u },
        { 15273158586344293478u, 1291124939043454294u }, { 13369007293925138595u, 2065799902469526871u },
        {  7005857020398200553u, 1652639921975621497u }, { 16672732060544291412u, 1322111937580497197u },
        { 11918976037903224966u, 2115379100128795516u }, {  5845832015580669650u, 1692303280103036413u },
        { 12055363241948356366u, 1353842624082429130u }, {   841837113407818570u, 2166148198531886609u },
        {  4362818505468165179u, 1732918558825509287u }, { 14558301248600263113u, 1386334847060407429u },
        { 12225235553534690011u, 2218135755296651887u }, {  2401490813343931363u, 1774508604237321510u },
        {  1921192650675145090u, 1419606883389857208u }, { 17831303500047873437u, 2271371013423771532u },
        {  6886345170554478103u, 1817096810739017226u }, {  1819727321701672159u, 1453677448591213781u },
        { 16213177116328979020u, 1162941958872971024u }, { 14873036941900635463u, 1860707134196753639u },
        { 15587778368262418694u, 1488565707357402911u }, {  8780873879868024632u, 1190852565885922329u },
        {  2981351763563108441u, 1905364105417475727u }, { 13453127855076217722u, 1524291284333980581u },
        {  7073153469319063855u, 1219433027467184465u }, { 11317045550910502167u, 1951092843947495144u },
        { 12742985255470312057u, 1560874275157996115u }, { 10194388204376249646u, 1248699420126396892u },
        {  1553625868034358140u, 1997919072202235028u }, {  8621598323911307159u, 1598335257761788022u },
        { 17965325103354776697u, 1278668206209430417u }, { 13987124906400001422u, 2045869129935088668u },
        {   121653480894270168u, 1636695303948070935u }, {    97322784715416134u, 1309356243158456748u },
        { 14913111714512307107u, 2094969989053530796u }, {  8241140556867935363u, 1675975991242824637u },
        { 17660958889720079260u, 1340780792994259709u }, { 17189487779326395846u, 2145249268790815535u },
        { 13751590223461116677u, 1716199415032652428u }, { 18379969808252713988u, 1372959532026121942u },
        { 14650556434236701088u, 2196735251241795108u }, {   652398703163629901u, 1757388200993436087u },
        { 11589965406756634890u, 1405910560794748869u }, {  7475898206584884855u, 2249456897271598191u },
        {  2291369750525997561u, 1799565517817278553u }, {  9211793429904618695u, 1439652414253822842u },
        { 18428218302589300235u, 2303443862806116547u }, {  7363877012587619542u, 1842755090244893238u },
        { 13269799239553916280u, 1474204072195914590u }, { 10615839391643133024u, 1179363257756731672u },
        {  2227947767661371545u, 1886981212410770676u }, { 16539753473096738529u, 1509584969928616540u },
        { 13231802778477390823u, 1207667975942893232u }, {  6413489186596184024u, 1932268761508629172u },
        { 16198837793502678189u, 1545815009206903337u }, {  5580372605318321905u, 1236652007365522670u },
        {  8928596168509315048u, 1978643211784836272u }, { 18210923379033183008u, 1582914569427869017u },
        {  7190041073742725760u, 1266331655542295214u }, {   436019273762630246u, 2026130648867672343u },
        {  7727513048493924843u, 1620904519094137874u }, {  9871359253537050198u, 1296723615275310299u },
        {  4726128361433549347u, 2074757784440496479u }, {  7470251503888749801u, 1659806227552397183u },
        { 13354898832594820487u, 1327844982041917746u }, { 13989140502667892133u, 2124551971267068394u },
        { 14880661216876224029u, 1699641577013654715u }, { 11904528973500979224u, 1359713261610923772u },
        {  4289851098633925465u, 2175541218577478036u }, { 18189276137874781665u, 1740432974861982428u },
        {  3483374466074094362u, 1392346379889585943u }, {  1884050330976640656u, 2227754207823337509u },
        {  5196589079523222848u, 1782203366258670007u }, { 15225317707844309248u, 1425762693006936005u },
        {  5913764258841343181u, 2281220308811097609u }, {  8420360221814984868u, 1824976247048878087u },
        { 17804334621677718864u, 1459980997639102469u }, { 17932816512084085415u, 1167984798111281975u },
        { 10245762345624985047u, 1868775676978051161u }, {  4507261061758077715u, 1495020541582440929u },
        {  7295157664148372495u, 1196016433265952743u }, {  7982903447895485668u, 1913626293225524389u },
        { 10075671573058298858u, 1530901034580419511u }, {  4371188443704728763u, 1224720827664335609u },
        { 14372599139411386667u, 1959553324262936974u }, { 15187428126271019657u, 1567642659410349579u },
        { 15839291315758726049u, 1254114127528279663u }, {  3206773216762499739u, 2006582604045247462u },
        { 13633465017635730761u, 1605266083236197969u }, { 14596120828850494932u, 1284212866588958375u },
        {  4907049252451240275u, 2054740586542333401u }, {   236290587219081897u, 1643792469233866721u },
        { 14946427728742906810u, 1315033975387093376u }, { 16535586736504830250u, 2104054360619349402u },
        {  5849771759720043554u, 1683243488495479522u }, { 15747863852001765813u, 1346594790796383617u },
        { 10439186904235184007u, 2154551665274213788u }, { 15730047152871967852u, 1723641332219371030u },
        { 12584037722297574282u, 1378913065775496824u }, {  9066413911450387881u, 2206260905240794919u },
        { 10942479943902220628u, 1765008724192635935u }, {  8753983955121776503u, 1412006979354108748u },
        { 10317025513452932081u, 2259211166966573997u }, {   874922781278525018u, 1807368933573259198u },
        {  8078635854506640661u, 1445895146858607358u }, { 13841606313089133175u, 1156716117486885886u },
        { 14767872471458792434u, 1850745787979017418u }, {   746251532941302978u, 1480596630383213935u },
        {   597001226353042382u, 1184477304306571148u }, { 15712597221132509104u, 1895163686890513836u },
        {  8880728962164096960u, 1516130949512411069u }, { 10793931984473187891u, 1212904759609928855u },
        { 17270291175157100626u, 1940647615375886168u }, {  2748186495899949531u, 1552518092300708935u },
        {  2198549196719959625u, 1242014473840567148u }, { 18275073973719576693u, 1987223158144907436u },
        { 10930710364233751031u, 1589778526515925949u }, { 12433917106128911148u, 1271822821212740759u },
        {  8826220925580526867u, 2034916513940385215u }, {  7060976740464421494u, 1627933211152308172u },
        { 16716827836597268165u, 1302346568921846537u }, { 11989529279587987770u, 2083754510274954460u },
        {  9591623423670390216u, 1667003608219963568u }, { 15051996368420132820u, 1333602886575970854u },
        { 13015147745246481542u, 2133764618521553367u }, {  3033420566713364587u, 1707011694817242694u },
        {  6116085268112601993u, 1365609355853794155u }, {  9785736428980163188u, 2184974969366070648u },
        { 15207286772667951197u, 1747979975492856518u }, {  1097782973908629988u, 1398383980394285215u },
        {  1756452758253807981u, 2237414368630856344u }, {  5094511021344956708u, 1789931494904685075u },
        {  4075608817075965366u, 1431945195923748060u }, {  6520974107321544586u, 2291112313477996896u },
        {  1527430471115325346u, 1832889850782397517u }, { 12289990821117991246u, 1466311880625918013u },
        { 17210690286378213644u, 1173049504500734410u }, {  9090360384495590213u, 1876879207201175057u },
        { 18340334751822203140u, 1501503365760940045u }, { 14672267801457762512u, 1201202692608752036u },
        { 16096930852848599373u, 1921924308174003258u }, {  1809498238053148529u, 1537539446539202607u },
        { 12515645034668249793u, 1230031557231362085u }, {  1578287981759648052u, 1968050491570179337u },
        { 12330676829633449412u, 1574440393256143469u }, { 13553890278448669853u, 1259552314604914775u },
        {  3239480371808320148u, 2015283703367863641u }, { 17348979556414297411u, 1612226962694290912u },
        {  6500486015647617283u, 1289781570155432730u }, { 10400777625036187652u, 2063650512248692368u },
        { 15699319729512770768u, 1650920409798953894u }, { 16248804598352126938u, 1320736327839163115u },
        {  7551343283653851484u, 2113178124542660985u }, {  6041074626923081187u, 1690542499634128788u },
        { 12211557331022285596u, 1352433999707303030u }, {  1091747655926105338u, 2163894399531684849u },
        {  4562746939482794594u, 1731115519625347879u }, {  7339546366328145998u, 1384892415700278303u },
        {  8053925371383123274u, 2215827865120445285u }, {  6443140297106498619u, 1772662292096356228u },
        { 12533209867169019542u, 1418129833677084982u }, {  5295740528502789974u, 2269007733883335972u },
        { 15304638867027962949u, 1815206187106668777u }, {  4865013464138549713u, 1452164949685335022u },
        { 14960057215536570740u, 1161731959748268017u }, {  9178696285890871890u, 1858771135597228828u },
        { 14721654658196518159u, 1487016908477783062u }, {  4398626097073393881u, 1189613526782226450u },
        {  7037801755317430209u, 1903381642851562320u }, {  5630241404253944167u, 1522705314281249856u },
        {   814844308661245011u, 1218164251424999885u }, {  1303750893857992017u, 1949062802279999816u },
        { 15800395974054034906u, 1559250241823999852u }, {  5261619149759407279u, 1247400193459199882u },
        { 12107939454356961969u, 1995840309534719811u }, {  5997002748743659252u, 1596672247627775849u },
        {  8486951013736837725u, 1277337798102220679u }, {  2511075177753209390u, 2043740476963553087u },
        { 13076906586428298482u, 1634992381570842469u }, { 14150874083884549109u, 1307993905256673975u },
        {  4194654460505726958u, 2092790248410678361u }, { 18113118827372222859u, 1674232198728542688u },
        {  3422448617672047318u, 1339385758982834151u }, { 16543964232501006678u, 2143017214372534641u },
        {  9545822571258895019u, 1714413771498027713u }, { 15015355686490936662u, 1371531017198422170u },
        {  5577825024675947042u, 2194449627517475473u }, { 11840957649224578280u, 1755559702013980378u },
        { 16851463748863483271u, 1404447761611184302u }, { 12204946739213931940u, 2247116418577894884u },
        { 13453306206113055875u, 1797693134862315907u }, {  3383947335406624054u, 1438154507889852726u },
        { 16482362180876329456u, 2301047212623764361u }, {  9496540929959153242u, 1840837770099011489u },
        { 11286581558709232917u, 1472670216079209191u }, {  5339916432225476010u, 1178136172863367353u },
        {  4854517476818851293u, 1885017876581387765u }, {  3883613981455081034u, 1508014301265110212u },
        { 14174937629389795797u, 1206411441012088169u }, { 11611853762797942306u, 1930258305619341071u },
        {  5600134195496443521u, 1544206644495472857u }, { 15548153800622885787u, 1235365315596378285u },
        {  6430302007287065643u, 1976584504954205257u }, { 16212288050055383484u, 1581267603963364205u },
        { 12969830440044306787u, 1265014083170691364u }, {  9683682259845159889u, 2024022533073106183u },
        { 15125643437359948558u, 1619218026458484946u }, {  8411165935146048523u, 1295374421166787957u },
        { 17147214310975587960u, 2072599073866860731u }, { 10028422634038560045u, 1658079259093488585u },
        {  8022738107230848036u, 1326463407274790868u }, {  9147032156827446534u, 2122341451639665389u },
        { 11006974540203867551u, 1697873161311732311u }, {  5116230817421183718u, 1358298529049385849u },
        { 15564666937357714594u, 2173277646479017358u }, {  1383687105660440706u, 1738622117183213887u },
        { 12174996128754083534u, 1390897693746571109u }, {  8411947361780802685u, 2225436309994513775u },
        {  6729557889424642148u, 1780349047995611020u }, {  5383646311539713719u, 1424279238396488816u },
        {  1235136468979721303u, 2278846781434382106u }, { 15745504434151418335u, 1823077425147505684u },
        { 16285752362063044992u, 1458461940118004547u }, {  5649904260166615347u, 1166769552094403638u },
        {  5350498001524674232u, 1866831283351045821u }, {   591049586477829062u, 1493465026680836657u },
        { 11540886113407994219u, 1194772021344669325u }, {    18673707743239135u, 1911635234151470921u },
        { 14772334225162232601u, 1529308187321176736u }, {  8128518565387875758u, 1223446549856941389u },
        {  1937583260394870242u, 1957514479771106223u }, {  8928764237799716840u, 1566011583816884978u },
        { 14521709019723594119u, 1252809267053507982u }, {  8477339172590109297u, 2004494827285612772u },
        { 17849917782297818407u, 1603595861828490217u }, {  6901236596354434079u, 1282876689462792174u },
        { 18420676183650915173u, 2052602703140467478u }, {  3668494502695001169u, 1642082162512373983u },
        { 10313493231639821582u, 1313665730009899186u }, {  9122891541139893884u, 2101865168015838698u },
        { 14677010862395735754u, 1681492134412670958u }, {   673562245690857633u, 1345193707530136767u }
    };

    static constexpr u64 POW5_TABLE_SIZE = 326;
    static constexpr u64 POW5_SPLIT[POW5_TABLE_SIZE][2] = {
        {                    0u, 1152921504606846976u }, {                    0u, 1441151880758558720u },
        {                    0u, 1801439850948198400u }, {                    0u, 2251799813685248000u },
        {                    0u, 1407374883553280000u }, {                    0u, 1759218604441600000u },
        {                    0u, 2199023255552000000u }, {                    0u, 1374389534720000000u },
        {                    0u, 1717986918400000000u }, {                    0u, 2147483648000000000u },
        {                    0u, 1342177280000000000u }, {                    0u, 1677721600000000000u },
        {                    0u, 2097152000000000000u }, {                    0u, 1310720000000000000u },
        {                    0u, 1638400000000000000u }, {                    0u, 2048000000000000000u },
        {                    0u, 1280000000000000000u }, {                    0u, 1600000000000000000u },
        {                    0u, 2000000000000000000u }, {                    0u, 1250000000000000000u },
        {                    0u, 1562500000000000000u }, {                    0u, 1953125000000000000u },
        {                    0u, 1220703125000000000u }, {                    0u, 1525878906250000000u },
        {                    0u, 1907348632812500000u }, {                    0u, 1192092895507812500u },
        {                    0u, 1490116119384765625u }, {  4611686018427387904u, 1862645149230957031u },
        {  9799832789158199296u, 1164153218269348144u }, { 12249790986447749120u, 1455191522836685180u },
        { 15312238733059686400u, 1818989403545856475u }, { 14528612397897220096u, 2273736754432320594u },
        { 13692068767113150464u, 1421085471520200371u }, { 12503399940464050176u, 1776356839400250464u },
        { 15629249925580062720u, 2220446049250313080u }, {  9768281203487539200u, 1387778780781445675u },
        {  7598665485932036096u, 1734723475976807094u }, {   274959820560269312u, 2168404344971008868u },
        {  9395221924704944128u, 1355252715606880542u }, {  2520655369026404352u, 1694065894508600678u },
        { 12374191248137781248u, 2117582368135750847u }, { 14651398557727195136u, 1323488980084844279u },
        { 13702562178731606016u, 1654361225106055349u }, {  3293144668132343808u, 2067951531382569187u },
        { 18199116482078572544u, 1292469707114105741u }, {  8913837547316051968u, 1615587133892632177u },
        { 15753982952572452864u, 2019483917365790221u }, { 12152082354571476992u, 1262177448353618888u },
        { 15190102943214346240u, 1577721810442023610u }, {  9764256642163156992u, 1972152263052529513u },
        { 17631875447420442880u, 1232595164407830945u }, {  8204786253993389888u, 1540743955509788682u },
        {  1032610780636961552u, 1925929944387235853u }, {  2951224747111794922u, 1203706215242022408u },
        {  3689030933889743652u, 1504632769052528010u }, { 13834660704216955373u, 1880790961315660012u },
        { 17870034976990372916u, 1175494350822287507u }, { 17725857702810578241u, 1469367938527859384u },
        {  3710578054803671186u, 1836709923159824231u }, {    26536550077201078u, 2295887403949780289u },
        { 11545800389866720434u, 1434929627468612680u }, { 14432250487333400542u, 1793662034335765850u },
        {  8816941072311974870u, 2242077542919707313u }, { 17039803216263454053u, 1401298464324817070u },
        { 12076381983474541759u, 1751623080406021338u }, {  5872105442488401391u, 2189528850507526673u },
        { 15199280947623720629u, 1368455531567204170u }, {  9775729147674874978u, 1710569414459005213u },
        { 16831347453020981627u, 2138211768073756516u }, {  1296220121283337709u, 1336382355046097823u },
        { 15455333206886335848u, 1670477943807622278u }, { 10095794471753144002u, 2088097429759527848u },
        {  6309871544845715001u, 1305060893599704905u }, { 12499025449484531656u, 1631326116999631131u },
        { 11012095793428276666u, 2039157646249538914u }, { 11494245889320060820u, 1274473528905961821u },
        {   532749306367912313u, 1593091911132452277u }, {  5277622651387278295u, 1991364888915565346u },
        {  7910200175544436838u, 1244603055572228341u }, { 14499436237857933952u, 1555753819465285426u },
        {  8900923260467641632u, 1944692274331606783u }, { 12480606065433357876u, 1215432671457254239u },
        { 10989071563364309441u, 1519290839321567799u }, {  9124653435777998898u, 1899113549151959749u },
        {  8008751406574943263u, 1186945968219974843u }, {  5399253239791291175u, 1483682460274968554u },
        { 15972438586593889776u, 1854603075343710692u }, {   759402079766405302u, 1159126922089819183u },
        { 14784310654990170340u, 1448908652612273978u }, {  9257016281882937117u, 1811135815765342473u },
        { 16182956370781059300u, 2263919769706678091u }, {  7808504722524468110u, 1414949856066673807u },
        {  5148944884728197234u, 1768687320083342259u }, {  1824495087482858639u, 2210859150104177824u },
        {  1140309429676786649u, 1381786968815111140u }, {  1425386787095983311u, 1727233711018888925u },
        {  6393419502297367043u, 2159042138773611156u }, { 13219259225790630210u, 1349401336733506972u },
        { 16524074032238287762u, 1686751670916883715u }, { 16043406521870471799u, 2108439588646104644u },
        {   803757039314269066u, 1317774742903815403u }, { 14839754354425000045u, 1647218428629769253u },
        {  4714634887749086344u, 2059023035787211567u }, {  9864175832484260821u, 1286889397367007229u },
        { 16941905809032713930u, 1608611746708759036u }, {  2730638187581340797u, 2010764683385948796u },
        { 10930020904093113806u, 1256727927116217997u }, { 18274212148543780162u, 1570909908895272496u },
        {  4396021111970173586u, 1963637386119090621u }, {  5053356204195052443u, 1227273366324431638u },
        { 15540067292098591362u, 1534091707905539547u }, { 14813398096695851299u, 1917614634881924434u },
        { 13870059828862294966u, 1198509146801202771u }, { 12725888767650480803u, 1498136433501503464u },
        { 15907360959563101004u, 1872670541876879330u }, { 14553786618154326031u, 1170419088673049581u },
        {  4357175217410743827u, 1463023860841311977u }, { 10058155040190817688u, 1828779826051639971u },
        {  7961007781811134206u, 2285974782564549964u }, { 14199001900486734687u, 1428734239102843727u },
        { 13137066357181030455u, 1785917798878554659u }, { 11809646928048900164u, 2232397248598193324u },
        { 16604401366885338411u, 1395248280373870827u }, { 16143815690179285109u, 1744060350467338534u },
        { 10956397575869330579u, 2180075438084173168u }, {  6847748484918331612u, 1362547148802608230u },
        { 17783057643002690323u, 1703183936003260287u }, { 17617136035325974999u, 2128979920004075359u },
        { 17928239049719816230u, 1330612450002547099u }, { 17798612793722382384u, 1663265562503183874u },
        { 13024893955298202172u, 2079081953128979843u }, {  5834715712847682405u, 1299426220705612402u },
        { 16516766677914378815u, 1624282775882015502u }, { 11422586310538197711u, 2030353469852519378u },
        { 11750802462513761473u, 1268970918657824611u }, { 10076817059714813937u, 1586213648322280764u },
        { 12596021324643517422u, 1982767060402850955u }, {  5566670318688504437u, 1239229412751781847u },
        {  2346651879933242642u, 1549036765939727309u }, {  7545000868343941206u, 1936295957424659136u },
        {  4715625542714963254u, 1210184973390411960u }, {  5894531928393704067u, 1512731216738014950u },
        { 16591536947346905892u, 1890914020922518687u }, { 17287239619732898039u, 1181821263076574179u },
        { 16997363506238734644u, 1477276578845717724u }, {  2799960309088866689u, 1846595723557147156u },
        { 10973347230035317489u, 1154122327223216972u }, { 13716684037544146861u, 1442652909029021215u },
        { 12534169028502795672u, 1803316136286276519u }, { 11056025267201106687u, 2254145170357845649u },
        { 18439230838069161439u, 1408840731473653530u }, { 13825666510731675991u, 1761050914342066913u },
        {  3447025083132431277u, 2201313642927583642u }, {  6766076695385157452u, 1375821026829739776u },
        {  8457595869231446815u, 1719776283537174720u }, { 10571994836539308519u, 2149720354421468400u },
        {  6607496772837067824u, 1343575221513417750u }, { 17482743002901110588u, 1679469026891772187u },
        { 17241742735199000331u, 2099336283614715234u }, { 15387775227926763111u, 1312085177259197021u },
        {  5399660979626290177u, 1640106471573996277u }, { 11361262242960250625u, 2050133089467495346u },
        { 11712474920277544544u, 1281333180917184591u }, { 10028907631919542777u, 1601666476146480739u },
        {  7924448521472040567u, 2002083095183100924u }, { 14176152362774801162u, 1251301934489438077u },
        {  3885132398186337741u, 1564127418111797597u }, {  9468101516160310080u, 1955159272639746996u },
        { 15140935484454969608u, 1221974545399841872u }, {   479425281859160394u, 1527468181749802341u },
        {  5210967620751338397u, 1909335227187252926u }, { 17091912818251750210u, 1193334516992033078u },
        { 12141518985959911954u, 1491668146240041348u }, { 15176898732449889943u, 1864585182800051685u },
        { 11791404716994875166u, 1165365739250032303u }, { 10127569877816206054u, 1456707174062540379u },
        {  8047776328842869663u, 1820883967578175474u }, {   836348374198811271u, 2276104959472719343u },
        {  7440246761515338900u, 1422565599670449589u }, { 13911994470321561530u, 1778206999588061986u },
        {  8166621051047176104u, 2222758749485077483u }, {  2798295147690791113u, 1389224218428173427u },
        { 17332926989895652603u, 1736530273035216783u }, { 17054472718942177850u, 2170662841294020979u },
        {  8353202440125167204u, 1356664275808763112u }, { 10441503050156459005u, 1695830344760953890u },
        {  3828506775840797949u, 2119787930951192363u }, {    86973725686804766u, 1324867456844495227u },
        { 13943775212390669669u, 1656084321055619033u }, {  3594660960206173375u, 2070105401319523792u },
        {  2246663100128858359u, 1293815875824702370u }, { 12031700912015848757u, 1617269844780877962u },
        {  5816254103165035138u, 2021587305976097453u }, {  5941001823691840913u, 1263492066235060908u },
        {  7426252279614801142u, 1579365082793826135u }, {  4671129331091113523u, 1974206353492282669u },
        {  5225298841145639904u, 1233878970932676668u }, {  6531623551432049880u, 1542348713665845835u },
        {  3552843420862674446u, 1927935892082307294u }, { 16055585193321335241u, 1204959932551442058u },
        { 10846109454796893243u, 1506199915689302573u }, { 18169322836923504458u, 1882749894611628216u },
        { 11355826773077190286u, 1176718684132267635u }, {  9583097447919099954u, 1470898355165334544u },
        { 11978871809898874942u, 1838622943956668180u }, { 14973589762373593678u, 2298278679945835225u },
        {  2440964573842414192u, 1436424174966147016u }, {  3051205717303017741u, 1795530218707683770u },
        { 13037379183483547984u, 2244412773384604712u }, {  8148361989677217490u, 1402757983365377945u },
        { 14797138505523909766u, 1753447479206722431u }, { 13884737113477499304u, 2191809349008403039u },
        { 15595489723564518921u, 1369880843130251899u }, { 14882676136028260747u, 1712351053912814874u },
        {  9379973133180550126u, 2140438817391018593u }, { 17391698254306313589u, 1337774260869386620u },
        {  3292878744173340370u, 1672217826086733276u }, {  4116098430216675462u, 2090272282608416595u },
        {   266718509671728212u, 1306420176630260372u }, {   333398137089660265u, 1633025220787825465u },
        {  5028433689789463235u, 2041281525984781831u }, { 10060300083759496378u, 1275800953740488644u },
        { 12575375104699370472u, 1594751192175610805u }, {  1884160825592049379u, 1993438990219513507u },
        { 17318501580490888525u, 1245899368887195941u }, {  7813068920331446945u, 1557374211108994927u },
        {  5154650131986920777u, 1946717763886243659u }, {   915813323278131534u, 1216698602428902287u },
        { 14979824709379828129u, 1520873253036127858u }, {  9501408849870009354u, 1901091566295159823u },
        { 12855909558809837702u, 1188182228934474889u }, {  2234828893230133415u, 1485227786168093612u },
        {  2793536116537666769u, 1856534732710117015u }, {  8663489100477123587u, 1160334207943823134u },
        {  1605989338741628675u, 1450417759929778918u }, { 11230858710281811652u, 1813022199912223647u },
        {  9426887369424876662u, 2266277749890279559u }, { 12809333633531629769u, 1416423593681424724u },
        { 16011667041914537212u, 1770529492101780905u }, {  6179525747111007803u, 2213161865127226132u },
        { 13085575628799155685u, 1383226165704516332u }, { 16356969535998944606u, 1729032707130645415u },
        { 15834525901571292854u, 2161290883913306769u }, {  2979049660840976177u, 1350806802445816731u },
        { 17558870131333383934u, 1688508503057270913u }, {  8113529608884566205u, 2110635628821588642u },
        {  9682642023980241782u, 1319147268013492901u }, { 16714988548402690132u, 1648934085016866126u },
        { 11670363648648586857u, 2061167606271082658u }, { 11905663298832754689u, 1288229753919426661u },
        {  1047021068258779650u, 1610287192399283327u }, { 15143834390605638274u, 2012858990499104158u },
        {  4853210475701136017u, 1258036869061940099u }, {  1454827076199032118u, 1572546086327425124u },
        {  1818533845248790147u, 1965682607909281405u }, {  3442426662494187794u, 1228551629943300878u },
        { 13526405364972510550u, 1535689537429126097u }, {  3072948650933474476u, 1919611921786407622u },
        { 15755650962115585259u, 1199757451116504763u }, { 15082877684217093670u, 1499696813895630954u },
        {  9630225068416591280u, 1874621017369538693u }, {  8324733676974063502u, 1171638135855961683u },
        {  5794231077790191473u, 1464547669819952104u }, {  7242788847237739342u, 1830684587274940130u },
        { 18276858095901949986u, 2288355734093675162u }, { 16034722328366106645u, 1430222333808546976u },
        {  1596658836748081690u, 1787777917260683721u }, {  6607509564362490017u, 2234722396575854651u },
        {  1823850468512862308u, 1396701497859909157u }, {  6891499104068465790u, 1745876872324886446u },
        { 17837745916940358045u, 2182346090406108057u }, {  4231062170446641922u, 1363966306503817536u },
        {  5288827713058302403u, 1704957883129771920u }, {  6611034641322878003u, 2131197353912214900u },
        { 13355268687681574560u, 1331998346195134312u }, { 16694085859601968200u, 1664997932743917890u },
        { 11644235287647684442u, 2081247415929897363u }, {  4971804045566108824u, 1300779634956185852u },
        {  6214755056957636030u, 1625974543695232315u }, {  3156757802769657134u, 2032468179619040394u },
        {  6584659645158423613u, 1270292612261900246u }, { 17454196593302805324u, 1587865765327375307u },
        { 17206059723201118751u, 1984832206659219134u }, {  6142101308573311315u, 1240520129162011959u },
        {  3065940617289251240u, 1550650161452514949u }, {  8444111790038951954u, 1938312701815643686u },
        {   665883850346957067u, 1211445438634777304u }, {   832354812933696334u, 1514306798293471630u },
        { 10263815553021896226u, 1892883497866839537u }, { 17944099766707154901u, 1183052186166774710u },
        { 13206752671529167818u, 1478815232708468388u }, { 16508440839411459773u, 1848519040885585485u },
        { 12623618533845856310u, 1155324400553490928u }, { 15779523167307320387u, 1444155500691863660u },
        {  1277659885424598868u, 1805194375864829576u }, {  1597074856780748586u, 2256492969831036970u },
        {  5609857803915355770u, 1410308106144398106u }, { 16235694291748970521u, 1762885132680497632u },
        {  1847873790976661535u, 2203606415850622041u }, { 12684136165428883219u, 1377254009906638775u },
        { 11243484188358716120u, 1721567512383298469u }, {   219297180166231438u, 2151959390479123087u },
        {  7054589765244976505u, 1344974619049451929u }, { 13429923224983608535u, 1681218273811814911u },
        { 12175718012802122765u, 2101522842264768639u }, { 14527352785642408584u, 1313451776415480399u },
        { 13547504963625622826u, 1641814720519350499u }, { 12322695186104640628u, 2052268400649188124u },
        { 16925056528170176201u, 1282667750405742577u }, {  7321262604930556539u, 1603334688007178222u },
        { 18374950293017971482u, 2004168360008972777u }, {  4566814905495150320u, 1252605225005607986u },
        { 14931890668723713708u, 1565756531257009982u }, {  9441491299049866327u, 1957195664071262478u },
        {  1289246043478778550u, 1223247290044539049u }, {  6223243572775861092u, 1529059112555673811u },
        {  3167368447542438461u, 1911323890694592264u }, {  1979605279714024038u, 1194577431684120165u },
        {  7086192618069917952u, 1493221789605150206u }, { 18081112809442173248u, 1866527237006437757u },
        { 13606538515115052232u, 1166579523129023598u }, {  7784801107039039482u, 1458224403911279498u },
        {   507629346944023544u, 1822780504889099373u }, {  5246222702107417334u, 2278475631111374216u },
        {  3278889188817135834u, 1424047269444608885u }, {  8710297504448807696u, 1780059086805761106u }
    };

    static constexpr u32 POW10_OFFSET_SIZE = 64;
    static constexpr u16 POW10_OFFSET[POW10_OFFSET_SIZE] = {
        0, 2, 5, 8, 12, 16, 21, 26, 32, 39,
        46, 54, 62, 71, 80, 90, 100, 111, 122, 134,
        146, 159, 173, 187, 202, 217, 233, 249, 266, 283,
        301, 319, 338, 357, 377, 397, 418, 440, 462, 485,
        508, 532, 556, 581, 606, 632, 658, 685, 712, 740,
        769, 798, 828, 858, 889, 920, 952, 984, 1017, 1050,
        1084, 1118, 1153, 1188
    };

    static constexpr u64 POW10_SPLIT[1224][3] = {
        {                    1u,    72057594037927936u,                    0u },
        {   699646928636035157u,             72057594u,                    0u },
        {                    1u,                    0u,                  256u },
        { 11902091922964236229u,        4722366482869u,                    0u },
        {  6760415703743915872u,                 4722u,                    0u },
        {                    1u,                    0u,             16777216u },
        { 13369850649504950658u,   309485009821345068u,                    0u },
        { 15151142278969419334u,            309485009u,                    0u },
        {                    1u,                    0u,          75511627776u },
        {  4635408826454083567u,  9437866644873197963u,                 1099u },
        { 12367138975830625353u,       20282409603651u,                    0u },
        {  7555853734021184432u,                20282u,                    0u },
        {                    1u,                    0u,         250037927936u },
        {  5171444645924616995u,   699646928636035156u,             72057594u },
        { 16672297533003297786u,  1329227995784915872u,                    0u },
        { 14479142226848862515u,           1329227995u,                    0u },
        {                    1u,                    0u,         181645213696u },
        { 12214193123817091081u, 11902091922964236228u,         114366482869u },
        { 16592893013671929435u,  6760415703743915871u,                 4722u },
        {  4549827147718617003u,       87112285931760u,                    0u },
        {  5274510003815168971u,                87112u,                    0u },
        {                    1u,                    0u,          44724781056u },
        {  9794971998307800535u, 13369850649504950657u,         209821345068u },
        { 14720142899209240169u, 15151142278969419333u,            309485009u },
        {  4300745446091561535u,  5708990770823839524u,                    0u },
        { 15197156861335443364u,           5708990770u,                    0u },
        {                    1u,                    0u,         139251286016u },
        { 13484604155038683037u,  4635408826454083566u,          67670423947u },
        {  8356963862052375699u, 12367138975830625352u,          58409603651u },
        {  5850852848337610021u,  7555853734021184431u,                20282u },
        {  2712780827214982050u,      374144419156711u,                    0u },
        {  7732076577307618052u,               374144u,                    0u },
        {                    1u,                    0u,          84280344576u },
        { 17296309485351745867u,  5171444645924616994u,         160903807060u },
        { 16598859101615853088u, 16672297533003297785u,         219784915872u },
        {  7469952526870444257u, 14479142226848862514u,           1329227995u },
        { 13531654022114669524u,  6073184580144670117u,                    1u },
        { 15757609704383306943u,          24519928653u,                    0u },
        {  9590990814237149590u,                   24u,                    0u },
        {                    1u,                    0u,         196662132736u },
        { 15408590707489433890u, 12214193123817091080u,          95899502532u },
        { 18332056844289122710u, 16592893013671929434u,         240246646623u },
        { 11114572877353986193u,  4549827147718617002u,          72285931760u },
        {  1703393793997526525u,  5274510003815168970u,                87112u },
        {  5082852056285196265u,     1606938044258990u,                    0u },
        {   816434266573722365u,              1606938u,                    0u },
        {                    1u,                    0u,         129530986496u },
        {  5736523019264798742u,  9794971998307800534u,          69797980545u },
        { 10129314776268243339u, 14720142899209240168u,          36233143877u },
        { 16511595775483995364u,  4300745446091561534u,          50823839524u },
        { 12367293405401453325u, 15197156861335443363u,           5708990770u },
        { 16934621733248854291u, 13078571300009428617u,                    5u },
        { 10278280417769171336u,         105312291668u,                    0u },
        {  5760764486226151240u,                  105u,                    0u },
        {                    1u,                    0u,         238731001856u },
        {  4128368337188369761u, 13484604155038683036u,          72453031918u },
        { 10240941003671005056u,  8356963862052375698u,         175317175368u },
        { 17933378316822368251u,  5850852848337610020u,         231147060143u },
        {  8346249813075698616u,  2712780827214982049u,         128419156711u },
        { 15906203609160902695u,  7732076577307618051u,               374144u },
        { 14525607416135386328u,     6901746346790563u,                    0u },
        {  6397156777364256320u,              6901746u,                    0u },
        {                    1u,                    0u,          34937634816u },
        { 16798760952716600048u, 17296309485351745866u,         249899825954u },
        {  2419982808370854967u, 16598859101615853087u,          50404946937u },
        {  2922947087773078956u,  7469952526870444256u,         165733552434u },
        { 15419220167069510190u, 13531654022114669523u,          77854221733u },
        {  3452124642157173416u, 15757609704383306942u,          24519928653u },
        {  5979700067267186899u,  9590990814237149589u,                   24u },
        {  4913998146922579597u,         452312848583u,                    0u },
        {  5771037749337678924u,                  452u,                    0u },
        {                    1u,                    0u,           8835301376u },
        {  3464734175350698519u, 15408590707489433889u,          90993782792u },
        {  9334527711335850125u, 18332056844289122709u,         170602522202u },
        {  7269882896518450106u, 11114572877353986192u,         202092341162u },
        {  1372511258182263196u,  1703393793997526524u,         174275541962u },
        {  7571228438575951046u,  5082852056285196264u,          26044258990u },
        {  2992506536646070406u,   816434266573722364u,              1606938u },
        {   524517896824344606u,    29642774844752946u,                    0u },
        { 15582941400898702773u,             29642774u,                    0u },
        {                    1u,                    0u,         214310977536u },
        {  3846112492507251066u,  5736523019264798741u,         104549111254u },
        { 16681117750123089487u, 10129314776268243338u,          62895095400u },
        { 14986314536556547267u, 16511595775483995363u,         163670432318u },
        {  2573712825027107389u, 12367293405401453324u,         137918027683u },
        {  7504855874008324928u, 16934621733248854290u,          84557186697u },
        {  9572138030626879787u, 10278280417769171335u,         105312291668u },
        {  8520676959353394843u,  5760764486226151239u,                  105u },
        { 13448984662897903496u,        1942668892225u,                    0u },
        { 12338883700918130648u,                 1942u,                    0u },
        {                    1u,                    0u,         156223799296u },
        {  2517285787892561600u,  4128368337188369760u,         146555162524u },
        {  4338831817635138103u, 10240941003671005055u,          36972170386u },
        {  1561495325934523196u, 17933378316822368250u,         161452451108u },
        { 12262635050079398786u,  8346249813075698615u,           3862277025u },
        { 11144065765517284188u, 15906203609160902694u,         163787434755u },
        {  1212260522471875711u, 14525607416135386327u,         242346790563u },
        {  9695352922247418869u,  6397156777364256319u,              6901746u },
        {  7227025834627242948u,   127314748520905380u,                    0u },
        {  9609008238705447829u,            127314748u,                    0u },
        {                    1u,                    0u,          74910662656u },
        {  3609144142396852269u, 16798760952716600047u,          31131187530u },
        { 11568848377382068865u,  2419982808370854966u,         224158453279u },
        { 10068303578029323957u,  2922947087773078955u,         211835877600u },
        { 11645070846862630231u, 15419220167069510189u,         190187140051u },
        { 12449386705878485055u,  3452124642157173415u,         149324160190u },
        { 15025619323517318418u,  5979700067267186898u,         199266388373u },
        { 14996237555047131272u,  4913998146922579596u,         196312848583u },
        { 10211005638256058413u,  5771037749337678923u,                  452u },
        {  1014743503555840530u,        8343699359066u,                    0u },
        { 12900897707145290678u,                 8343u,                    0u },
        {                    1u,                    0u,          33187823616u },
        {  4718003016239473662u,  3464734175350698518u,         149506025761u },
        { 14865830648693666725u,  9334527711335850124u,         144394101141u },
        { 14754517212823091778u,  7269882896518450105u,         252074403984u },
        { 11113946551474911901u,  1372511258182263195u,         232410437116u },
        {  1963520352638130630u,  7571228438575951045u,         252162224104u },
        { 13342587341404964200u,  2992506536646070405u,          50028434172u },
        {  6240392545013573291u,   524517896824344605u,          22844752946u },
        { 14377490861349714758u, 15582941400898702772u,             29642774u },
        {  1717863312631397839u,   546812681195752981u,                    0u },
        {  3611005143890591770u,            546812681u,                    0u },
        {                    1u,                    0u,          21208498176u },
        { 13168252824351245504u,  3846112492507251065u,         138904285205u },
        {   735883891883379688u, 16681117750123089486u,         227812409738u },
        { 10609203866866106404u, 14986314536556547266u,          12139521251u },
        { 12358191111890306470u,  2573712825027107388u,          18406839052u },
        { 15229916368406413528u,  7504855874008324927u,         135518906642u },
        {  7241424335568075942u,  9572138030626879786u,          71461906823u },
        {  6049715868779871913u,  8520676959353394842u,          65729070919u },
        {  2000548404719336762u, 13448984662897903495u,         150668892225u },
        {  1410974761895205301u, 12338883700918130647u,                 1942u },
        { 16000132467694084868u,       35835915874844u,                    0u },
        { 16894908866816792556u,                35835u,                    0u },
        {                    1u,                    0u,          96136462336u },
        {   589096329272056762u,  2517285787892561599u,         127235208544u },
        {  7097729792403256904u,  4338831817635138102u,         250084648831u },
        {  8553736750439287020u,  1561495325934523195u,         183664758778u },
        {  2114152625261065696u, 12262635050079398785u,          38604121015u },
        {  9817523680007641224u, 11144065765517284187u,         215065716774u },
        { 13047215537500048015u,  1212260522471875710u,          63525586135u },
        { 16755544192002345880u,  9695352922247418868u,         164391777855u },
        {  6930119832670648356u,  7227025834627242947u,          60520905380u },
        { 14560698131901886167u,  9609008238705447828u,            127314748u },
        { 16408020927503338035u,  2348542582773833227u,                    0u },
        { 14274703510609809116u,           2348542582u,                    0u },
        {                    1u,                    0u,         239195652096u },
        { 16428432973129962470u,  3609144142396852268u,          54627148527u },
        {  3721112279790863774u, 11568848377382068864u,         171545803830u },
        { 18032764903259620753u, 10068303578029323956u,          45631280555u },
        { 18058455550468776079u, 11645070846862630230u,         167674882605u },
        { 15692090139033993190u, 12449386705878485054u,         210814540455u },
        {   389416944300619393u, 15025619323517318417u,         140812947666u },
        { 12009691357260487293u, 14996237555047131271u,          75553539724u },
        { 13494259174449809900u, 10211005638256058412u,          90055009355u },
        { 18288583400616279877u,  1014743503555840529u,         151699359066u },
        {     7216107869057472u, 12900897707145290677u,                 8343u },
        { 17237061291959073878u,      153914086704665u,                    0u },
        {  1599418782488783273u,               153914u,                    0u },
        {                    1u,                    0u,          22255763456u },
        {  9565464987240335777u,  4718003016239473661u,         140805878294u },
        {   857713933775880687u, 14865830648693666724u,         185799843980u },
        {  4621617820081363356u, 14754517212823091777u,         155602488249u },
        {  9630162611715632528u, 11113946551474911900u,         197106442651u },
        {  9283986497984645815u,  1963520352638130629u,         133723303109u },
        {  8981807745082630996u, 13342587341404964199u,          29338292357u },
        { 18350140531565934622u,  6240392545013573290u,         180779405341u },
        {  4411619033127524143u, 14377490861349714757u,          21093125556u },
        {  1852297584111266889u,  1717863312631397838u,           9195752981u },
        { 11746243463811666096u,  3611005143890591769u,            546812681u },
        {  6335244004343789147u, 10086913586276986678u,                    0u },
        {  5109502367228239844u,          10086913586u,                    0u },
        {  1603272682579847821u,                   10u,                    0u },
        {                    1u,                    0u,         121713852416u },
        {  6609546910952910052u, 13168252824351245503u,          78039892345u },
        {  3911171343112928288u,   735883891883379687u,         194575126094u },
        {  5254510615100863555u, 10609203866866106403u,          60669938882u },
        {  3881927570803887650u, 12358191111890306469u,          63825615420u },
        {  6379348759607163190u, 15229916368406413527u,          42392558399u },
        { 14595733737222406466u,  7241424335568075941u,         154327955754u },
        { 14670223432002373542u,  6049715868779871912u,         135108449946u },
        {  4045087795619708513u,  2000548404719336761u,         215076489095u },
        { 12598467307137142718u,  1410974761895205300u,          28867368919u },
        {   734704388050777108u, 16000132467694084867u,         251915874844u },
        {  5682201693687285822u, 16894908866816792555u,                35835u },
        { 11048712694145438788u,      661055968790248u,                    0u },
        { 17871025777010319485u,               661055u,                    0u },
        {                    1u,                    0u,         191031934976u },
        { 15268761435931663695u,   589096329272056761u,          54384768703u },
        {  5016238054648555438u,  7097729792403256903u,          59463698998u },
        { 14236047313993899750u,  8553736750439287019u,         129114608443u },
        {  6957759675154690848u,  2114152625261065695u,          91532209025u },
        { 18439367135478514473u,  9817523680007641223u,         126707290971u },
        {  8539004472540641041u, 13047215537500048014u,         244908319870u },
        {  1908462039431738399u, 16755544192002345879u,         195375682548u },
        {   714690453250792146u,  6930119832670648355u,         148789337027u },
        { 13782189447673929633u, 14560698131901886166u,          11889480596u },
        {  3584742913798803164u, 16408020927503338034u,         118773833227u },
        {  4347581515245125291u, 14274703510609809115u,           2348542582u },
        { 16836742268156371392u,  6429475823218628948u,                    2u },
        { 11764082328865615308u,          43322963970u,                    0u },
        {  5957633711383291746u,                   43u,                    0u },
        {                    1u,                    0u,          44890587136u },
        {  9917186842884466953u, 16428432973129962469u,         128201721900u },
        {  4751011869809829335u,  3721112279790863773u,         180977558144u },
        { 11068497969931435029u, 18032764903259620752u,          86978950836u },
        { 17118056985122509954u, 18058455550468776078u,          62850669910u },
        { 14607066080907684459u, 15692090139033993189u,          17021110334u },
        { 11768892370493391107u,   389416944300619392u,         135651046673u },
        {  4043396447647747170u, 12009691357260487292u,          44731525255u },
        {  1670341095362518057u, 13494259174449809899u,          17991426092u },
        {  3190817644167043165u, 18288583400616279876u,         181000391185u },
        { 10425820027224322486u,     7216107869057471u,          25934422965u },
        { 13139964660506311565u, 17237061291959073877u,          58086704665u },
        {  2297772885416059937u,  1599418782488783272u,               153914u },
        {  7677687919964523763u,     2839213766779714u,                    0u },
        { 14144589152747892828u,              2839213u,                    0u },
        {                    1u,                    0u,         253518544896u },
        { 17069730341503660290u,  9565464987240335776u,         164046496765u },
        { 18167423787163077107u,   857713933775880686u,          65250538404u },
        {  3765746945827805904u,  4621617820081363355u,         156522052161u },
        { 10241734342430761691u,  9630162611715632527u,         197503285916u },
        { 13345717282537140784u,  9283986497984645814u,         103486904773u },
        {  9313926784816939953u,  8981807745082630995u,         170994763111u },
        {   550974205049535019u, 18350140531565934621u,          69239154346u },
        {  4494692285504086222u,  4411619033127524142u,         206100413253u },
        {  1134308559863725587u,  1852297584111266888u,          25636765134u },
        { 17587558045116130233u, 11746243463811666095u,          54343434265u },
        {  9817142032346161594u,  6335244004343789146u,          50276986678u },
        {  6071944935834172568u,  5109502367228239843u,          10086913586u },
        { 11564168293299416955u,  1603272682579847820u,                   10u },
        { 12458266507226064437u,         186070713419u,                    0u },
        {  1304432355328256915u,                  186u,                    0u },
        {                    1u,                    0u,         191358304256u },
        { 15946798815542087355u,  6609546910952910051u,         231212025023u },
        { 12082566083831286138u,  3911171343112928287u,          35284847591u },
        { 11449623684706196411u,  5254510615100863554u,         165210439715u },
        { 17518743620362604446u,  3881927570803887649u,         215345825189u },
        {  9451061563087633805u,  6379348759607163189u,         165791236311u },
        { 13191114787623314926u, 14595733737222406465u,         168795274405u },
        {  8367349876734474799u, 14670223432002373541u,          57219284648u },
        {  6544253801674393507u,  4045087795619708512u,         180682964281u },
        { 16113906253336597498u, 12598467307137142717u,           3039828404u },
        { 10294087136797312392u,   734704388050777107u,         235308032771u },
        {  9127173070014462803u,  5682201693687285821u,         232598951915u },
        { 16266900839595484952u, 11048712694145438787u,          63968790248u },
        {  3299745387370952632u, 17871025777010319484u,               661055u },
        { 12061115182604399189u,    12194330274671844u,                    0u },
        {  5066801222582989646u,             12194330u,                    0u },
        {                    1u,                    0u,         185827721216u },
        {  7568423425299591513u, 15268761435931663694u,          71271930809u },
        { 16561505984665207377u,  5016238054648555437u,         235771737671u },
        {  4329114621856906245u, 14236047313993899749u,         223377180907u },
        {  1477500474861899139u,  6957759675154690847u,         135999600095u },
        { 16891579639263969684u, 18439367135478514472u,         142462900359u },
        {  4684451357140027420u,  8539004472540641040u,         151103457934u },
        { 14727186580409080709u,  1908462039431738398u,          35038743447u },
        { 15864176859687308834u,   714690453250792145u,         214747133987u },
        {  1755486942842684438u, 13782189447673929632u,          50194329302u },
        { 17417077516652710041u,  3584742913798803163u,         219235682866u },
        {  4290982361913532783u,  4347581515245125290u,          84912721627u },
        { 11826659981004351409u, 16836742268156371391u,           2637732180u },
        {   932930645678090820u, 11764082328865615307u,          43322963970u },
        { 12707792781328052617u,  5957633711383291745u,                   43u },
        { 16491596426880311906u,         799167628880u,                    0u },
        {  3092207065214166010u,                  799u,                    0u },
        {                    1u,                    0u,         229537611776u },
        {  8142946531605512550u,  9917186842884466952u,         157257552869u },
        {  5328402096432654515u,  4751011869809829334u,         144600024477u },
        {  1932004361303814512u, 11068497969931435028u,         142927971728u },
        {  2511477647985517771u, 17118056985122509953u,         229791850638u },
        { 17451375493324716694u, 14607066080907684458u,         128637992933u },
        {  9489266854478998489u, 11768892370493391106u,         124219192960u },
        {  8803053132063235169u,  4043396447647747169u,         235090549372u },
        { 16198682197142616773u,  1670341095362518056u,          68172974571u },
        { 13696242485403414202u,  3190817644167043164u,         191565184836u },
        { 16409082426079859931u, 10425820027224322485u,          85712318911u },
        { 11653410736879597610u, 13139964660506311564u,         168124562517u },
        { 13589514120653213261u,  2297772885416059936u,          66416208296u },
        {  8032934885905905774u,  7677687919964523762u,         173766779714u },
        {  2753021350129449273u, 14144589152747892827u,              2839213u },
        { 16974897459201404133u,    52374249726338269u,                    0u },
        { 13398576176159101589u,             52374249u,                    0u },
        {                    1u,                    0u,         160925351936u },
        { 10284586955251725351u, 17069730341503660289u,         238984858016u },
        {  5294476488634150891u, 18167423787163077106u,         155204141550u },
        { 15833244538135063323u,  3765746945827805903u,         143555205531u },
        { 10348512742273116664u, 10241734342430761690u,         182723472783u },
        { 13658504610142595663u, 13345717282537140783u,          83504908982u },
        { 11956362239240850266u,  9313926784816939952u,          29029868371u },
        { 13415901703662731781u,   550974205049535018u,          46243657757u },
        {  5161774027546852762u,  4494692285504086221u,          72061490990u },
        { 15274384838790587711u,  1134308559863725586u,         175953423432u },
        { 14233354597679374929u, 17587558045116130232u,          90532188335u },
        {  4274656492162486921u,  9817142032346161593u,         227329160794u },
        { 12040276505541795046u,  6071944935834172567u,         140626894819u },
        { 13238307206256765457u, 11564168293299416954u,          75675363980u },
        { 12850161204172713271u, 12458266507226064436u,         186070713419u },
        { 17531777095001445154u,  1304432355328256914u,                  186u },
        {  5623628114515245990u,        3432398830065u,                    0u },
        {  7357116143579573377u,                 3432u,                    0u },
        {                    1u,                    0u,         227864477696u },
        {  3555734177475596582u, 15946798815542087354u,          31654997219u },
        { 14001876724756424382u, 12082566083831286137u,          66620685343u },
        { 18159905057231476140u, 11449623684706196410u,          33949692994u },
        {  5585207679308509467u, 17518743620362604445u,          53512343073u },
        { 13948697622866724672u,  9451061563087633804u,          65715091765u },
        {  9807691927739036432u, 13191114787623314925u,         165453594945u },
        { 15818010096140820918u,  8367349876734474798u,          96354764709u },
        {  5629845624785010943u,  6544253801674393506u,         189873536608u },
        {  9517635131137734707u, 16113906253336597497u,          19558043581u },
        {   619338244618780585u, 10294087136797312391u,          61494785043u },
        { 11632367007491958899u,  9127173070014462802u,          67881830461u },
        { 12083314261009739916u, 16266900839595484951u,         124178879555u },
        { 16880538609458881650u,  3299745387370952631u,         228653834364u },
        { 17404223674486504228u, 12061115182604399188u,          26274671844u },
        {  7089067015287185433u,  5066801222582989645u,             12194330u },
        {  2592264228029443648u,   224945689727159819u,                    0u },
        { 13413731084370224440u,            224945689u,                    0u },
        {                    1u,                    0u,          78410285056u },
        {  9323915941641553425u,  7568423425299591512u,         173897801038u },
        { 12155831029092699564u, 16561505984665207376u,         229234681773u },
        { 17397171276588232676u,  4329114621856906244u,          31080095461u },
        { 11874560617553253769u,  1477500474861899138u,          40915694367u },
        { 13444839516837727954u, 16891579639263969683u,          16253944616u },
        { 16994416043584590671u,  4684451357140027419u,          30798362384u },
        { 15879694502877015730u, 14727186580409080708u,         209859998750u },
        {  4234647645735263359u, 15864176859687308833u,         160095165137u },
        {  7978589901512919496u,  1755486942842684437u,         219944181664u },
        {  6114237175390859894u, 17417077516652710040u,         170232614619u },
        {  8658612872088282708u,  4290982361913532782u,         191641124522u },
        { 10253813330683324853u, 11826659981004351408u,         203050574271u },
        { 13289465061747830991u,   932930645678090819u,          97688890827u },
        {  4123165538545565412u, 12707792781328052616u,          80894011233u },
        {  7846417485927038481u, 16491596426880311905u,          31167628880u },
        { 10562273346358018864u,  3092207065214166009u,                  799u },
        {  2691512658346619120u,       14742040721959u,                    0u },
        {   751187558544605998u,                14742u,                    0u },
        {                    1u,                    0u,           8441430016u },
        {  3757709791947931308u,  8142946531605512549u,         214288853256u },
        {  3452755398462519465u,  5328402096432654514u,          20104734166u },
        {  3105818720159874523u,  1932004361303814511u,         129136147476u },
        { 16859138458894499364u,  2511477647985517770u,         106946040961u },
        { 12271894740606233755u, 17451375493324716693u,           2514414186u },
        {  5429638071845793701u,  9489266854478998488u,          97477214466u },
        {   145278150038876889u,  8803053132063235168u,          40878132321u },
        {  9050266019724932450u, 16198682197142616772u,          92742474792u },
        { 11907016253451490866u, 13696242485403414201u,         181889538140u },
        {  2472757296513770735u, 16409082426079859930u,         140631732661u },
        { 10558733798178239360u, 11653410736879597609u,          32736689036u },
        { 15917322570831255850u, 13589514120653213260u,         242435466272u },
        { 12254334656791355238u,  8032934885905905773u,          91149241586u },
        {  7869542424662730262u,  2753021350129449272u,         221920211035u },
        {  1378558986933000253u, 16974897459201404132u,         233726338269u },
        { 13521405041909411105u, 13398576176159101588u,             52374249u },
        {  3206744593298092012u,   966134380754314586u,                    0u },
        { 13914648122214918505u,            966134380u,                    0u },
        {                    1u,                    0u,           1557528576u },
        {  1235541077112082496u, 10284586955251725350u,         242287014145u },
        { 12014985518315533846u,  5294476488634150890u,         207858321906u },
        {  1561535086344155741u, 15833244538135063322u,         218560993999u },
        { 12761747276316224577u, 10348512742273116663u,          47740429018u },
        {  9745594781103966137u, 13658504610142595662u,         176648155695u },
        { 17514238702394846785u, 11956362239240850265u,          42727277488u },
        {  2428898913707151713u, 13415901703662731780u,         205279820330u },
        {    71666709959904945u,  5161774027546852761u,          18828026061u },
        {  4049380135452919193u, 15274384838790587710u,         184771591698u },
        { 18422240861777453733u, 14233354597679374928u,         185231729592u },
        {  2914504416394425696u,  4274656492162486920u,         151652704697u },
        { 12721377795748989418u, 12040276505541795045u,         122717650071u },
        {  2626074459217717422u, 13238307206256765456u,          52696608634u },
        {  4261529925046307655u, 12850161204172713270u,         146950399540u },
        { 11536038685430305586u, 17531777095001445153u,         241304857490u },
        { 12555757789435162768u,  5623628114515245989u,         104398830065u },
        { 11905178684546080059u,  7357116143579573376u,                 3432u },
        { 14032797718924543051u,       63316582777114u,                    0u },
        { 10750340288005853484u,                63316u,                    0u },
        {                    1u,                    0u,         186192756736u },
        {  9660290106216358253u,  3555734177475596581u,         121759043258u },
        { 14820142034615351103u, 14001876724756424381u,         186984450425u },
        { 12674041783707777619u, 18159905057231476139u,         157302774714u },
        { 15386686816442679994u,  5585207679308509466u,         140756160413u },
        {  5679510383719146248u, 13948697622866724671u,         237531676044u },
        {  1391101719248678506u,  9807691927739036431u,          46857496045u },
        {  3364596672173710517u, 15818010096140820917u,         162305194542u },
        { 11276509210104319732u,  5629845624785010942u,         249515952034u },
        {  5316312656902630164u,  9517635131137734706u,         135033574393u },
        { 17470981304473644647u,   619338244618780584u,          82630591879u },
        {  7373293636384920591u, 11632367007491958898u,          23655037778u },
        {  7616810902585191937u, 12083314261009739915u,         183915095831u },
        { 12740295655921903924u, 16880538609458881649u,          84943484855u },
        { 18366635945916526940u, 17404223674486504227u,          77384299092u },
        {  4472171448243407067u,  7089067015287185432u,          11140526925u },
        {   229592460858185629u,  2592264228029443647u,          25727159819u },
        { 12749672866417114996u, 13413731084370224439u,            224945689u },
        {  9452256722867098693u,  4149515568880992958u,                    0u },
        { 16251451636418604634u,           4149515568u,                    0u },
        {                    1u,                    0u,          88505450496u },
        {  4515791283442995454u,  9323915941641553424u,          80658968920u },
        { 13306155670047701346u, 12155831029092699563u,           4943102544u },
        {  4456930152933417601u, 17397171276588232675u,         130643721220u },
        {  9089157128546489637u, 11874560617553253768u,         147728846210u },
        { 12437332180345515840u, 13444839516837727953u,          27921269139u },
        {  3433060408790452524u, 16994416043584590670u,         132860839963u },
        {  8275594526021936172u, 15879694502877015729u,          33229560708u },
        {  3846512444641107689u,  4234647645735263358u,          21432520225u },
        {  6210962618469046250u,  7978589901512919495u,         152331453461u },
        {  7272858906616296575u,  6114237175390859893u,         110469384344u },
        {  3710743300451225347u,  8658612872088282707u,         176555860334u },
        {  6424677242672030600u, 10253813330683324852u,          67720423344u },
        { 11485842256170301862u, 13289465061747830990u,         136223517251u },
        {  7355797963557024308u,  4123165538545565411u,          97425355144u },
        {  6358188982569427273u,  7846417485927038480u,         249572581985u },
        { 12475094728768767402u, 10562273346358018863u,          39145907193u },
        { 17288154837907896183u,  2691512658346619119u,         150040721959u },
        {  2983850577727105262u,   751187558544605997u,                14742u },
        { 13918604635001185935u,      271942652322184u,                    0u },
        { 12033220395769876327u,               271942u,                    0u },
        {                    1u,                    0u,         101203705856u },
        {  5782377197813462997u,  3757709791947931307u,         178187174245u },
        { 17732139848231399226u,  3452755398462519464u,         111168366770u },
        {  3628839527415562921u,  3105818720159874522u,         202913935727u },
        {  3188692267613601004u, 16859138458894499363u,         149665260746u },
        {  5168130193478377352u, 12271894740606233754u,         216294341269u },
        { 12556227529405091290u,  5429638071845793700u,          96007875544u },
        { 15087090312791441192u,   145278150038876888u,         196490615904u },
        { 10281804758610642494u,  9050266019724932449u,         185645480644u },
        { 14238177586158586580u, 11907016253451490865u,         218134048441u },
        {  7107927498217678128u,  2472757296513770734u,          41572390106u },
        {  3845814658485364450u, 10558733798178239359u,          76862879785u },
        {   714293333681725946u, 15917322570831255849u,         109664308812u },
        { 16766172658649116982u, 12254334656791355237u,          56426608749u },
        {   812461421432632215u,  7869542424662730261u,         228074731832u },
        { 15218024718633799196u,  1378558986933000252u,         148732996836u },
        {  8110797782612805146u, 13521405041909411104u,          90173837972u },
        { 15941193964933529227u,  3206744593298092011u,         108754314586u },
        { 14144280602323277933u, 13914648122214918504u,            966134380u },
        { 15072402647813125245u, 17822033662586700072u,                    0u },
        { 10822706091283369889u,          17822033662u,                    0u },
        { 15163844593710966731u,                   17u,                    0u },
        {                    1u,                    0u,          38066978816u },
        {  2408529687792073670u,  1235541077112082495u,         234651333670u },
        {  3980682212356510808u, 12014985518315533845u,          26084650986u },
        {  4202670442792148519u,  1561535086344155740u,         247691815706u },
        {  9419583343154651922u, 12761747276316224576u,          78528309751u },
        { 16359166491570434575u,  9745594781103966136u,          89949448782u },
        { 12567727056384237385u, 17514238702394846784u,           4131670873u },
        {  2068388267923286639u,  2428898913707151712u,         153003885060u },
        {  5689135844565021196u,    71666709959904944u,          62219517337u },
        {  3104061965171139313u,  4049380135452919192u,          80998671678u },
        {  7955173880156328016u, 18422240861777453732u,         136157995600u },
        {  1445179403240833754u,  2914504416394425695u,         229689627272u },
        { 12538201164459126715u, 12721377795748989417u,          16142359781u },
        {  7580606719088482667u,  2626074459217717421u,          54231018000u },
        {  8168318283218819755u,  4261529925046307654u,          33625369910u },
        {  5249615277755961676u, 11536038685430305585u,         165680648993u },
        {  6312997372068219831u, 12555757789435162767u,         128645381029u },
        {  9183815417025176703u, 11905178684546080058u,          26760719488u },
        { 10683849953373876937u, 14032797718924543050u,          84582777114u },
        { 17175012155615667568u, 10750340288005853483u,                63316u },
        { 18003508288378896912u,     1167984798111281u,                    0u },
        { 14722554560950996951u,              1167984u,                    0u },
        {                    1u,                    0u,          37523685376u },
        { 15059324482416394930u,  9660290106216358252u,         189803401509u },
        {  4134778595813308312u, 14820142034615351102u,         171687061181u },
        { 16321118342639660948u, 12674041783707777618u,          26834113963u },
        {  1523550293123468805u, 15386686816442679993u,          63307886874u },
        {  8016371634569878509u,  5679510383719146247u,          15075411775u },
        {  9884220139611134110u,  1391101719248678505u,         181182395151u },
        {  7218073002727840414u,  3364596672173710516u,         254611300789u },
        { 16062235669481359233u, 11276509210104319731u,          50288197886u },
        { 15558048660560338002u,  5316312656902630163u,         168947103794u },
        {  8394398745765058609u, 17470981304473644646u,         114399707048u },
        {  5693296366442904274u,  7373293636384920590u,         139412908146u },
        { 11783494675061161358u,  7616810902585191936u,         113690652811u },
        { 13377293110865447894u, 12740295655921903923u,          35995657329u },
        { 12840734051093062130u, 18366635945916526939u,          24242436899u },
        {  7009868331566697505u,  4472171448243407066u,          63012446232u },
        {  5019690705031194477u,   229592460858185628u,          55691161151u },
        {  8608277240439804984u, 12749672866417114995u,         190512407863u },
        { 12172482590657749222u,  9452256722867098692u,          48880992958u },
        { 16613484892678771990u, 16251451636418604633u,           4149515568u },
        {  5721488662757049244u,  2758075434182769113u,                    4u },
        {   386931106438877039u,          76545051729u,                    0u },
        { 10054429752182825659u,                   76u,                    0u },
        {                    1u,                    0u,          16244801536u },
        {  8634592106137071313u,  4515791283442995453u,         171721328144u },
        { 12626356501369830731u, 13306155670047701345u,         227241610667u },
        {  4803333258178976933u,  4456930152933417600u,         136492724195u },
        { 13613083223558209297u,  9089157128546489636u,         209674229128u },
        { 16106967997237446989u, 12437332180345515839u,          78186106577u },
        { 14832921244380020170u,  3433060408790452523u,         177448620878u },
        { 13774024637717231397u,  8275594526021936171u,         126208519857u },
        {  9673012968505228885u,  3846512444641107688u,         199336696958u },
        {  5391832334264815667u,  6210962618469046249u,         117394262471u },
        { 16514436292632703088u,  7272858906616296574u,          83201159797u },
        { 12025036352783454153u,  3710743300451225346u,         180348282451u },
        {  7059867105311401050u,  6424677242672030599u,         206622648756u },
        { 12769210631552594670u, 11485842256170301861u,         227398758606u },
        {  8328873878884556145u,  7355797963557024307u,          16344678115u },
        {  1016565892414238685u,  6358188982569427272u,          47676276240u },
        {  9662978461927250281u, 12475094728768767401u,         239937192751u },
        { 13729967277551868112u, 17288154837907896182u,          45161754863u },
        {  6371593776693359475u,  2983850577727105261u,         136754529069u },
        { 17617208110845643245u, 13918604635001185934u,          70652322184u },
        { 14960960225633086797u, 12033220395769876326u,               271942u },
        { 12090634301321662558u,     5016456510113118u,                    0u },
        {  9409926148478635503u,              5016456u,                    0u },
        {                    1u,                    0u,         171313463296u },
        {  4307062684900157136u,  5782377197813462996u,         168961261227u },
        { 15300759383869911853u, 17732139848231399225u,         218196719784u },
        { 16007534237643445447u,  3628839527415562920u,          35172859354u },
        {  7138502295759677634u,  3188692267613601003u,         154280164899u },
        {  8218537071653683708u,  5168130193478377351u,         164680674458u },
        {  2254219416760329296u, 12556227529405091289u,         216817872804u },
        {  3057410459568460683u, 15087090312791441191u,          97557377752u },
        {  8217810929938874370u, 10281804758610642493u,          49771853153u },
        { 11741126472498340929u, 14238177586158586579u,         238385321521u },
        {  1175325363726654805u,  7107927498217678127u,         127208482030u },
        {  9428843070696730900u,  3845814658485364449u,          41038721919u },
        { 12662500978715131896u,   714293333681725945u,         101908896041u },
        {  6443045597035184564u, 16766172658649116981u,          21044043621u },
        {  1921385512639171183u,   812461421432632214u,          60824970773u },
        { 10469475094355551399u, 15218024718633799195u,          32439687228u },
        { 14679174489076953574u,  8110797782612805145u,         235864173856u },
        { 11853074234719825644u, 15941193964933529226u,         104766762987u },
        {  8270896886596139124u, 14144280602323277932u,          40817076584u },
        { 16532667046659118126u, 15072402647813125244u,         254586700072u },
        {   148341279888833483u, 10822706091283369888u,          17822033662u },
        { 10364629296397276041u, 15163844593710966730u,                   17u },
        { 14265682585545771671u,         328758493846u,                    0u },
        { 13991741872911347878u,                  328u,                    0u },
        {                    1u,                    0u,          63130566656u },
        { 14029045786848724433u,  2408529687792073669u,          21215793215u },
        {  4005878521026842341u,  3980682212356510807u,          92227827221u },
        {  3428326338640386488u,  4202670442792148518u,          64510636636u },
        {  1010001558294829380u,  9419583343154651921u,         184886832192u },
        {  2012063724327403418u, 16359166491570434574u,          64681297848u },
        { 10997154538851372612u, 12567727056384237384u,          96112127552u },
        {  1917749645489607898u,  2068388267923286638u,         176308408672u },
        {  9763872523711218805u,  5689135844565021195u,         152168271536u },
        { 15875699078454059311u,  3104061965171139312u,         164431250840u },
        { 10966529452671276106u,  7955173880156328015u,          95078343332u },
        { 18073244132105736913u,  1445179403240833753u,         233679697247u },
        {  4435241176994913173u, 12538201164459126714u,         173410945513u },
        {  5464400086219074323u,  7580606719088482666u,          70442805421u },
        {  2445909179323258812u,  8168318283218819754u,          49284582214u },
        {   873962058644121211u,  5249615277755961675u,         143342228273u },
        { 16675872194112650857u,  6312997372068219830u,          58497855631u },
        { 10680102689274800355u,  9183815417025176702u,          74579172666u },
        {  2370498083108897524u, 10683849953373876936u,          43931059274u },
        { 15354400521451334666u, 17175012155615667567u,          49975972139u },
        {   259991949657381021u, 18003508288378896911u,         112798111281u },
        { 10335286558772966917u, 14722554560950996950u,              1167984u },
        { 16337526653906757263u,    21545516652742137u,                    0u },
        { 12040967163702784894u,             21545516u,                    0u },
        {                    1u,                    0u,         108816367616u },
        {  3373309160242342187u, 15059324482416394929u,          62224146796u },
        { 13639841054510584221u,  4134778595813308311u,          82884769598u },
        { 15898855427739708031u, 16321118342639660947u,         185082591826u },
        {  4544387940067005419u,  1523550293123468804u,           7434568377u },
        {  5281598644835398575u,  8016371634569878508u,         105535824647u },
        { 13675642405083408835u,  9884220139611134109u,         180391292521u },
        {  3973392623768015721u,  7218073002727840413u,         243870735540u },
        {  4491285101509114191u, 16062235669481359232u,          19843403507u },
        { 15002304272810270500u, 15558048660560338001u,         102455061267u },
        { 17325098540619893468u,  8394398745765058608u,          14308634214u },
        {  1137212864974584822u,  5693296366442904273u,            638784526u },
        {  2619406097224859078u, 11783494675061161357u,          51725184512u },
        {  8281347529729293732u, 13377293110865447893u,          91696097587u },
        { 11344719666795450104u, 12840734051093062129u,         218380005723u },
        { 17283870506679425783u,  7009868331566697504u,         156272117978u },
        { 11054210518010603775u,  5019690705031194476u,         115466655644u },
        {  6399455551799092885u,  8608277240439804983u,          68659871603u },
        { 12930529916573967170u, 12172482590657749221u,          89900618820u },
        { 14550097052337552404u, 16613484892678771989u,         217310162521u },
        { 12487632712206414748u,  5721488662757049243u,          81020975577u },
        {  5791017277843595715u,   386931106438877038u,          76545051729u },
        { 10227264183449036113u, 10054429752182825658u,                   76u },
        {  2006055278511721441u,        1412006979354u,                    0u },
        {   128746359043876333u,                 1412u,                    0u },
        {                    1u,                    0u,         253468082176u },
        {  7408146306870995754u,  8634592106137071312u,          97684476157u },
        {  8299024588195267962u, 12626356501369830730u,         128260389217u },
        {  1497052939192040881u,  4803333258178976932u,          36737966720u },
        { 16771714264265803747u, 13613083223558209296u,          63873160484u },
        {   142988846654429432u, 16106967997237446988u,          43804094271u },
        { 11839838367716104145u, 14832921244380020169u,          43746691371u },
        {  6019646776647679765u, 13774024637717231396u,         232524375083u },
        {  4611972391702034948u,  9673012968505228884u,         233292291816u },
        { 16447182322205429545u,  5391832334264815666u,         126895249385u },
        {  2113477168726764245u, 16514436292632703087u,           2651878526u },
        {  3536261187802311516u, 12025036352783454152u,         135382716162u },
        { 18444381860986709854u,  7059867105311401049u,         165692220295u },
        {  4734315730275909838u, 12769210631552594669u,          51451509157u },
        {  9974936316849658174u,  8328873878884556144u,          72055108147u },
        { 11864423681540657642u,  1016565892414238684u,         169523831112u },
        {  8207245621417902667u,  9662978461927250280u,         118744303017u },
        {  7992526918695295028u, 13729967277551868111u,         237345404790u },
        {  8679354522130259987u,  6371593776693359474u,         142955030765u },
        {  6065763799692166461u, 17617208110845643244u,         102811035278u },
        { 18143341109049024976u, 14960960225633086796u,          94655434598u },
        { 15242492331283350570u, 12090634301321662557u,         136510113118u },
        {  9986352353182266963u,  9409926148478635502u,              5016456u },
        { 17340463289911536077u,    92537289398950870u,                    0u },
        {  7359344614214233035u,             92537289u,                    0u },
        {                    1u,                    0u,         212233486336u },
        {   419091135888749535u,  4307062684900157135u,          57829455828u },
        {  1073142712661309790u, 15300759383869911852u,         168867770169u },
        { 11076438902195672286u, 16007534237643445446u,         235386978984u },
        {  1820390940081322073u,  7138502295759677633u,         135445527787u },
        { 18417808973944523597u,  8218537071653683707u,         217122201479u },
        { 10251294197731582957u,  2254219416760329295u,          39165742553u },
        {  1502394029870156428u,  3057410459568460682u,          61445488423u },
        {   321014853559106075u,  8217810929938874369u,         211636487741u },
        {  2390953058510591778u, 11741126472498340928u,          47063714515u },
        { 10685224265907994087u,  1175325363726654804u,         225511138607u },
        {  5967405799190505023u,  9428843070696730899u,         249686435553u },
        { 11210723659228214761u, 12662500978715131895u,          53349278201u },
        { 12327123641078462773u,  6443045597035184563u,         150104158517u },
        {  1709976940107894237u,  1921385512639171182u,          27567551382u },
        { 16607686590938553511u, 10469475094355551398u,          25795759643u },
        { 18332088094272679457u, 14679174489076953573u,         138642556441u },
        {  2946170632136780882u, 11853074234719825643u,         108448366218u },
        {  4824449494694383419u,  8270896886596139123u,         124896237676u },
        { 17008332258693407134u, 16532667046659118125u,         160008041596u },
        {  1773419466622750661u,   148341279888833482u,         202561867680u },
        {  3892343466023784379u, 10364629296397276040u,         150773344202u },
        { 12001571085575422796u, 14265682585545771670u,          72758493846u },
        { 12933506765500977582u, 13991741872911347877u,                  328u },
        { 11884830007749143734u,        6064523798049u,                    0u },
        {  9662368568096205337u,                 6064u,                    0u },
        {                    1u,                    0u,         197760516096u },
        { 16801499925276664442u, 14029045786848724432u,          87217159109u },
        { 10492407990787637084u,  4005878521026842340u,          38185849943u },
        {  7673849751013230269u,  3428326338640386487u,          17054752294u },
        {  6046724489853072367u,  1010001558294829379u,          14109074193u },
        {  3723941391207507903u,  2012063724327403417u,          72596156942u },
        { 16844122108860347659u, 10997154538851372611u,         110103961416u },
        { 10622020182694668027u,  1917749645489607897u,          11529300590u },
        {  8741198820686854862u,  9763872523711218804u,         240860623371u },
        {  6855480461211306807u, 15875699078454059310u,          79594496752u },
        { 10005708458011566304u, 10966529452671276105u,         217979752527u },
        {  8932093106442919061u, 18073244132105736912u,         186240434905u },
        {  9062763476260756743u,  4435241176994913172u,         106296225722u },
        { 13664977682032775521u,  5464400086219074322u,         170132593002u },
        {  1078499125430623453u,  2445909179323258811u,          75047377578u },
        {  6554586738078431161u,   873962058644121210u,         182904000843u },
        { 12177313698643242883u, 16675872194112650856u,         126578969526u },
        { 16615072271904633953u, 10680102689274800354u,         200128504958u },
        { 16375404983106569285u,  2370498083108897523u,         111832363720u },
        { 13552251831473522729u, 15354400521451334665u,          15014094191u },
        {  8330500218412111874u,   259991949657381020u,         214560277007u },
        {  7044338079053294004u, 10335286558772966916u,         249885659094u },
        {  2688849443046530184u, 16337526653906757262u,          44652742137u },
        {   855940991879596845u, 12040967163702784893u,             21545516u },
        {  7344363609485825662u,   397444631628981487u,                    0u },
        { 11602660525134634992u,            397444631u,                    0u },
        {                    1u,                    0u,         177182867456u },
        { 16945343208344873835u,  3373309160242342186u,         151739417265u },
        {  9617992661337889145u, 13639841054510584220u,         147861878679u },
        { 18280344933262742088u, 15898855427739708030u,           4246351763u },
        {  5179975582362777795u,  4544387940067005418u,         236286316036u },
        {  1798918997870037130u,  5281598644835398574u,         157741358060u },
        {  6327667344756325883u, 13675642405083408834u,         157215398045u },
        { 18380327574124007701u,  3973392623768015720u,         128243473053u },
        { 18015447557304295289u,  4491285101509114190u,          81813276544u },
        { 10315590748073249878u, 15002304272810270499u,          48939195473u },
        {  7697916092577993382u, 17325098540619893467u,         209061648432u },
        {  3124132817942110723u,  1137212864974584821u,         141141998289u },
        {  7448238998520507049u,  2619406097224859077u,         213448932749u },
        { 13892823322374205297u,  8281347529729293731u,         241614998485u },
        { 11042137840046332564u, 11344719666795450103u,          32936960497u },
        { 10513966307445593804u, 17283870506679425782u,         108599249952u },
        {  9388437460943526958u, 11054210518010603774u,          55346915180u },
        { 10967228614677896228u,  6399455551799092884u,         229700965431u },
        {  2310996671540235542u, 12930529916573967169u,          21788762341u },
        {  4989110555003898587u, 14550097052337552403u,         155676955925u },
        { 16271452421983657679u, 12487632712206414747u,         110313931675u },
        {  9523160181437090473u,  5791017277843595714u,         186554421102u },
        { 13137707423765072250u, 10227264183449036112u,          26108748474u },
        { 16846859744221860705u,  2006055278511721440u,         132006979354u },
        {  7767140033449795569u,   128746359043876332u,                 1412u },
        { 17169456915721160017u,       26046931378436u,                    0u },
        { 17180899661833327819u,                26046u,                    0u },
        {                    1u,                    0u,         208401596416u },
        { 17572520700934791416u,  7408146306870995753u,          74449891024u },
        { 17968798858233825417u,  8299024588195267961u,         164081155402u },
        { 15338423313945305609u,  1497052939192040880u,          16909196452u },
        { 17895321323836726301u, 16771714264265803746u,          76007751440u },
        {   814069333008965773u,   142988846654429431u,         201641838924u },
        {  7200328959852723947u, 11839838367716104144u,          36326325705u },
        {   759884557248133773u,  6019646776647679764u,          84250015524u },
        { 13410165861863974851u,  4611972391702034947u,          50891603540u },
        {  6278452420856351570u, 16447182322205429544u,         111114571826u },
        {  9072115382556676442u,  2113477168726764244u,         200191701103u },
        {  2755882551854926563u,  3536261187802311515u,          89999871944u },
        {  8496072611504649269u, 18444381860986709853u,         237256647769u },
        {  4122009033579215815u,  4734315730275909837u,         112540742381u },
        { 10222217724450527221u,  9974936316849658173u,         220643171696u },
        {  2064539481554006325u, 11864423681540657641u,         104444915676u },
        {  7935605886598063693u,  8207245621417902666u,         207433275752u },
        {  7805147585347548429u,  7992526918695295027u,         114470508751u },
        {  5709020905457661273u,  8679354522130259986u,         236328825714u },
        { 16257370307404906674u,  6065763799692166460u,          76983552492u },
        { 14971258192939373646u, 18143341109049024975u,          93826297164u },
        {  1133404845901376390u, 15242492331283350569u,         238541361245u },
        {  9460827548162822047u,  9986352353182266962u,         214940028398u },
        {  1273897659779791346u, 17340463289911536076u,         201398950870u },
        {  7833262224435092783u,  7359344614214233034u,             92537289u },
        {  3033420566713364587u,  1707011694817242694u,                    0u },
        { 15075466825360349103u,           1707011694u,                    0u },
        {                    1u,                    0u,         207022718976u },
        {  2484134775182816690u,   419091135888749534u,          44058175183u },
        { 18400539815335991277u,  1073142712661309789u,         198600454956u },
        {   485494064952118286u, 11076438902195672285u,         193098683590u },
        { 17577048805241314891u,  1820390940081322072u,         251998431425u },
        {  2863946907557583807u, 18417808973944523596u,          79555723771u },
        { 13045307417786230800u, 10251294197731582956u,         138081444943u },
        { 12032088871615097766u,  1502394029870156427u,           1017402250u },
        {  8848763446997690580u,   321014853559106074u,          64129613825u },
        { 10031289150307672684u,  2390953058510591777u,          84579247168u },
        { 11592215575498656563u, 10685224265907994086u,          19323493716u },
        { 15894436747956898388u,  5967405799190505022u,         247607734547u },
        {  2091546719588500923u, 11210723659228214760u,         179668254711u },
        {  5863809244813756109u, 12327123641078462772u,         110092698035u },
        { 11303008753675411245u,  1709976940107894236u,         166900304494u },
        { 13238426537506910532u, 16607686590938553510u,         229993784486u },
        { 17258458071023005565u, 18332088094272679456u,         235159712229u },
        {  8385733444777075179u,  2946170632136780881u,         115261533931u },
        {  9530757096163247300u,  4824449494694383418u,          45922023539u },
        { 14423000845391072217u, 17008332258693407133u,         202096137261u },
        { 10953140011159884311u,  1773419466622750660u,         136211004362u },
        { 12228340237948264127u,  3892343466023784378u,         150650606472u },
        { 11279134946966259189u, 12001571085575422795u,         165701126806u },
        { 14640097792684582651u, 12933506765500977581u,          33644277925u },
        {  6232313315128656728u, 11884830007749143733u,         176523798049u },
        { 16136121832933322088u,  9662368568096205336u,                 6064u },
        { 15074767079673358271u,      111870718431542u,                    0u },
        { 13252722804829281908u,               111870u,                    0u },
        {                    1u,                    0u,         208910811136u },
        {  7740175894281560509u, 16801499925276664441u,         228568794576u },
        { 15670495392425593226u, 10492407990787637083u,         183416000228u },
        { 15152257626756992778u,  7673849751013230268u,          67327793591u },
        {  4090073428152440422u,  6046724489853072366u,         153201875267u },
        { 14450327772834205584u,  3723941391207507902u,          67913121689u },
        {  4466091895542494216u, 16844122108860347658u,         217575820867u },
        { 10454115378553795377u, 10622020182694668026u,         116473861337u },
        {  2267817233475657788u,  8741198820686854861u,          46371636340u },
        {  5500455702636497521u,  6855480461211306806u,          73542410542u },
        { 15178768299492252549u, 10005708458011566303u,         208484209737u },
        {  7062359872332045590u,  8932093106442919060u,         148491293392u },
        { 12297347290027942576u,  9062763476260756742u,          18740779924u },
        {  8030124596941085588u, 13664977682032775520u,         187058465554u },
        {  6526656990996654843u,  1078499125430623452u,         122355324859u },
        {  6254287345256979850u,  6554586738078431160u,         104660133498u },
        {  6642007136244870032u, 12177313698643242882u,         226900704872u },
        {  2027592955437164718u, 16615072271904633952u,         243887712482u },
        {   942718349157325567u, 16375404983106569284u,           9734669043u },
        { 14617066671884002278u, 13552251831473522728u,         156451597321u },
        {  6831631114396133348u,  8330500218412111873u,           4381874332u },
        { 14603040013386939258u,  7044338079053294003u,         142145762820u },
        {  9906106765319401103u,  2688849443046530183u,         125046400654u },
        {  1396179595609933063u,   855940991879596844u,         239398138749u },
        { 11524884268464976417u,  7344363609485825661u,          23628981487u },
        {   382929570730827274u, 11602660525134634991u,            397444631u },
        {  6109721884461301381u,  7331559403129590068u,                    0u },
        {  2390514825000339691u,           7331559403u,                    0u },
        {  6116191454763441755u,                    7u,                    0u },
        {                    1u,                    0u,          42918608896u },
        { 11598868771099176310u, 16945343208344873834u,         156521392426u },
        { 14449966445520085105u,  9617992661337889144u,         126990979484u },
        { 11675595287405614726u, 18280344933262742087u,         234280807038u },
        { 15860796398550489897u,  5179975582362777794u,         174097519594u },
        { 16180408435245829662u,  1798918997870037129u,         194343023534u },
        { 13756992797154950706u,  6327667344756325882u,         104996399554u },
        {  8830551328786758466u, 18380327574124007700u,          78976619368u },
        { 16699955256560951264u, 18015447557304295288u,          35559209294u },
        { 10038983627153402074u, 10315590748073249877u,         219417304867u },
        { 15085100736692127346u,  7697916092577993381u,         245169359579u },
        { 10007783780289711125u,  3124132817942110722u,         197403769845u },
        { 17596907048353602192u,  7448238998520507048u,         163753131461u },
        { 13530650344896573509u, 13892823322374205296u,         247598595491u },
        {  6337724853398437005u, 11042137840046332563u,         246569963255u },
        { 12768885008904063297u, 10513966307445593803u,         254508948214u },
        {  2759773619512884114u,  9388437460943526957u,         148594534654u },
        {  8434364600126655292u, 10967228614677896227u,          65125279380u },
        {  3843827521199949338u,  2310996671540235541u,          19270460225u },
        {  4661660852957808994u,  4989110555003898586u,         155882077203u },
        { 15298044134177324417u, 16271452421983657678u,         194516251547u },
        {  7747773274913338217u,  9523160181437090472u,          80712196546u },
        { 10348785912020632966u, 13137707423765072249u,         224913270096u },
        {  4175372293197190170u, 16846859744221860704u,         236421057504u },
        { 11326064156813083145u,  7767140033449795568u,           4930758124u },
        {  8100407170505981763u, 17169456915721160016u,         190931378436u },
        {  1706556116319916846u, 17180899661833327818u,                26046u },
        { 15028897280749641942u,      480481077043500u,                    0u },
        {  1421201742071739121u,               480481u,                    0u },
        {                    1u,                    0u,          41952608256u },
        {  8480737406125178272u, 17572520700934791415u,         121974090537u },
        { 10947205650755620361u, 17968798858233825416u,         176831497593u },
        {   868577942165647781u, 15338423313945305608u,         226970107312u },
        { 16017710019091388479u, 17895321323836726300u,         247044130786u },
        {  6610879150827623375u,   814069333008965772u,         208390330615u },
        { 12110095866223762092u,  7200328959852723946u,          20041193424u },
        {  7756802952949470775u,   759884557248133772u,           3726966548u },
        {  2941800790804618759u, 13410165861863974850u,          40340355587u },
        { 11703600274199927522u,  6278452420856351569u,         212491800360u },
        {   806737539257940346u,  9072115382556676441u,          91149396692u },
        { 14579028397110132023u,  2755882551854926562u,          93460573019u },
        { 14247808875344366934u,  8496072611504649268u,         205223454557u },
        {  9713379923695279513u,  4122009033579215814u,          61554147533u },
        {  2246428675703313877u, 10222217724450527220u,         233111918909u },
        {  3549783776592680620u,  2064539481554006324u,          74430190057u },
        { 12645029747929213033u,  7935605886598063692u,          51423117898u },
        { 16279009267476580506u,  7805147585347548428u,          18309486643u },
        {   343358782242907186u,  5709020905457661272u,          60881313810u },
        { 10077054739085890321u, 16257370307404906673u,         207811593532u },
        { 10526715404712173586u, 14971258192939373645u,          41061441999u },
        { 11438715865125144243u,  1133404845901376389u,          82512872489u },
        {  5040916178827294801u,  9460827548162822046u,         204069058130u },
        { 16643761637275849508u,  1273897659779791345u,         202424641996u },
        {  4852542977279030386u,  7833262224435092782u,          70164442058u },
        {  7883373066544387129u,  3033420566713364586u,         110817242694u },
        { 16699064314768500978u, 15075466825360349102u,           1707011694u },
        {  6805863634444817214u, 13042063791413317777u,                    1u },
        {  2266540253968903500u,          31488807865u,                    0u },
        {  9016913589137908810u,                   31u,                    0u },
        {                    1u,                    0u,         222134665216u },
        { 11654451024602552034u,  2484134775182816689u,          93997495262u },
        {  5299013208454526793u, 18400539815335991276u,         221026318685u },
        { 14918550373926182540u,   485494064952118285u,          88952853725u },
        {  6225552657491071054u, 17577048805241314890u,          76155254872u },
        { 10344713496596235785u,  2863946907557583806u,         236707187532u },
        { 12972405634433280209u, 13045307417786230799u,         139652260844u },
        { 12911885282402784945u, 12032088871615097765u,          26479692427u },
        {  6934311832970995868u,  8848763446997690579u,          33543797274u },
        {  9975729197003430461u, 10031289150307672683u,         230628415265u },
        {  1982857556803548935u, 11592215575498656562u,          62861639142u },
        {  2095735223386298223u, 15894436747956898387u,         232113382974u },
        {  7110931538347639365u,  2091546719588500922u,          52317877736u },
        { 15822183724630969535u,  5863809244813756108u,         220612737332u },
        { 16931982690156327501u, 11303008753675411244u,         166717656540u },
        {  6740069226761666110u, 13238426537506910531u,          32935582886u },
        {  3138792961008474902u, 17258458071023005564u,          81454591520u },
        { 12154594426971851390u,  8385733444777075178u,          58516663377u },
        { 15780127219221910902u,  9530757096163247299u,         157781872442u },
        { 16421541930960194381u, 14423000845391072216u,         196593770909u },
        {  7485894627196740576u, 10953140011159884310u,         186662899652u },
        {  8897269432694476707u, 12228340237948264126u,          75611443130u },
        { 17189823634941678805u, 11279134946966259188u,         173793641291u },
        {  9585582064286255216u, 14640097792684582650u,         181337854381u },
        { 12835472279575022097u,  6232313315128656727u,          24874740917u },
        {  6776016669542754608u, 16136121832933322087u,          54817204760u },
        { 18340015775620871027u, 15074767079673358270u,         254718431542u },
        {  5254188752292365830u, 13252722804829281907u,               111870u },
        {  6798802596750151183u,     2063650512248692u,                    0u },
        {  9449320530215272000u,              2063650u,                    0u },
        {                    1u,                    0u,         121419595776u },
        { 17110720482574968811u,  7740175894281560508u,          91849499257u },
        { 16172441693558688213u, 15670495392425593225u,         188821405531u },
        {  6234654946353717320u, 15152257626756992777u,         238221723324u },
        { 11180283100679445438u,  4090073428152440421u,         190783353838u },
        { 14852260031176961272u, 14450327772834205583u,          10242107326u },
        {  4481533167346438750u,  4466091895542494215u,         250566718730u },
        {  4269718344362365664u, 10454115378553795376u,         205122938618u },
        { 11520029752381101466u,  2267817233475657787u,          54298180301u },
        { 16778682550309368417u,  5500455702636497520u,         223822842678u },
        {  9687587467301363608u, 15178768299492252548u,         148382851295u },
        { 10093971076828497318u,  7062359872332045589u,           6666640532u },
        {  1913763026490934696u, 12297347290027942575u,          96435313926u },
        { 12701450127613557000u,  8030124596941085587u,         220353810784u },
        {  8974572160711134644u,  6526656990996654842u,         184339045596u },
        {  9890000077336694124u,  6254287345256979849u,         130360063928u },
        {  4292326716201059148u,  6642007136244870031u,          96109916034u },
        { 14644519175104337420u,  2027592955437164717u,          68051104864u },
        {  5051178622270136798u,   942718349157325566u,          40792392772u },
        {   675983118348065839u, 14617066671884002277u,           1370343464u },
        {  4431647660065117244u,  6831631114396133347u,         179791632385u },
        {  8316115180008411962u, 14603040013386939257u,         135537011123u },
        {  9621158095544965602u,  9906106765319401102u,          44075687047u },
        { 15283478958951102072u,  1396179595609933062u,         125624765228u },
        { 13981553073094447813u, 11524884268464976416u,         239020758653u },
        {  4558368743929911607u,   382929570730827273u,          52331208687u },
        { 15217004469858477791u,  6109721884461301380u,         235129590068u },
        { 11589190369996515737u,  2390514825000339690u,           7331559403u },
        {  3670624237398152929u,  6116191454763441754u,                    7u },
        { 13471713758418039777u,         135243399970u,                    0u },
        {  4489936967610296411u,                  135u,                    0u },
        {                    1u,                    0u,         106628775936u },
        {  9052049303222747950u, 11598868771099176309u,         120783334250u },
        {  1011330006193020538u, 14449966445520085104u,          71632935288u },
        { 17412075644359478612u, 11675595287405614725u,         194859815495u },
        {  6358678384745980468u, 15860796398550489896u,         137877141698u },
        { 15262353928842850919u, 16180408435245829661u,         250745768073u },
        { 11145257686438581736u, 13756992797154950705u,          20478705146u },
        {  1600562031807691890u,  8830551328786758465u,         120905306388u },
        {  6775147337046626724u, 16699955256560951263u,          85544214392u },
        { 15772127322106297822u, 10038983627153402073u,         165817764949u },
        {  4141472200527441474u, 15085100736692127345u,           2542523045u },
        { 18246007807879281267u, 10007783780289711124u,         168953930242u },
        {   960746958654787123u, 17596907048353602191u,         112733498024u },
        { 11355981212264408477u, 13530650344896573508u,         147343568752u },
        {  1573078209576251481u,  6337724853398437004u,         203692202643u },
        {  6245294478780491367u, 12768885008904063296u,          45149607627u },
        {  7523292955659721510u,  2759773619512884113u,          35457227821u },
        { 14454736751015226505u,  8434364600126655291u,          21208374307u },
        {  7219786377781411316u,  3843827521199949337u,         218252709141u },
        { 10597123082209392431u,  4661660852957808993u,         206829308634u },
        {  6922353544343010714u, 15298044134177324416u,         168420007630u },
        { 14317523356293377430u,  7747773274913338216u,         121561008808u },
        {  4057766168681892717u, 10348785912020632965u,          96226347385u },
        { 15214083611901244045u,  4175372293197190169u,         240613987168u },
        {  8390569016883950721u, 11326064156813083144u,          80439123952u },
        { 10680472538208175055u,  8100407170505981762u,         202092512592u },
        { 12173567833130544927u,  1706556116319916845u,          44814718154u },
        {  1386341248286610026u, 15028897280749641941u,         225077043500u },
        { 12487300952797237352u,  1421201742071739120u,               480481u },
        {  2614759871804869720u,     8863311460481781u,                    0u },
        {  8494389567327729477u,              8863311u,                    0u },
        {                    1u,                    0u,         247459741696u },
        {  6260469580539185878u,  8480737406125178271u,         136593449207u },
        { 17818573101084525841u, 10947205650755620360u,           8047085704u },
        {  2201029069927307150u,   868577942165647780u,          28868321800u },
        { 10397997613804897039u, 16017710019091388478u,         140358376476u },
        { 14269915965770103741u,  6610879150827623374u,         234656489612u },
        { 16776139909196366727u, 12110095866223762091u,         140420497130u },
        {  6246513436385199720u,  7756802952949470774u,         194159475340u },
        {  2926026498821554288u,  2941800790804618758u,          81634453442u },
        { 15725499391028340982u, 11703600274199927521u,          89043733329u },
        {  8576577277771450827u,   806737539257940345u,         226790330713u },
        { 15523351176022259335u, 14579028397110132022u,          52772375266u },
        {  4775158829429176134u, 14247808875344366933u,         198526563380u },
        { 10141817222123532462u,  9713379923695279512u,         244121779142u },
        { 12847658900242624586u,  2246428675703313876u,          52192434164u },
        { 13708197964460514655u,  3549783776592680619u,          76685488436u },
        {  1951540006613246932u, 12645029747929213032u,          12882486860u },
        {  9979297327280092199u, 16279009267476580505u,          88018613516u },
        { 15381307706282553684u,   343358782242907185u,         177546278232u },
        { 10037428657543061177u, 10077054739085890320u,          77570654385u },
        {  2584877324547208668u, 10526715404712173585u,         133620094029u },
        {  1126624732730703576u, 11438715865125144242u,         158273268613u },
        {  1501064139624981020u,  5040916178827294800u,         241902260126u },
        {  5219661484955306109u, 16643761637275849507u,          46263056881u },
        {  5336997298570282212u,  4852542977279030385u,         106427358510u },
        { 12191131175733833362u,  7883373066544387128u,         174905258090u },
        {  3707068178994436536u, 16699064314768500977u,         145368946606u },
        {  5045484691732942022u,  6805863634444817213u,         185122869393u },
        { 14847900542908711232u,  2266540253968903499u,          31488807865u },
        {  9097257915916965135u,  9016913589137908809u,                   31u },
        {  2472027983230314217u,         580865979874u,                    0u },
        { 15974509111133272205u,                  580u,                    0u },
        {                    1u,                    0u,         177631789056u },
        { 12099486841948187399u, 11654451024602552033u,         236287260081u },
        {  5319910566029976328u,  5299013208454526792u,          13808736236u },
        { 11549214421017285864u, 14918550373926182539u,          74337487885u },
        {  1998791413186046700u,  6225552657491071053u,         190560788042u },
        { 17075171930090011210u, 10344713496596235784u,          15703235518u },
        { 15158296003813501474u, 12972405634433280208u,         165699954703u },
        {  1360083178079384115u, 12911885282402784944u,         211375909797u },
        {  6167980558592741158u,  6934311832970995867u,         107540785363u },
        {  3630180428124865653u,  9975729197003430460u,          50107490923u },
        {  2276550099763657677u,  1982857556803548934u,          83113610034u },
        {   407006713016100655u,  2095735223386298222u,         186385484371u },
        { 14242579061653496002u,  7110931538347639364u,         204857722298u },
        { 17944493332678643704u, 15822183724630969534u,          44917884620u },
        {   987185901870869452u, 16931982690156327500u,          67365379884u },
        {  5578665155415167745u,  6740069226761666109u,         124170154307u },
        {  4849210377429577536u,  3138792961008474901u,         234658901884u },
        { 10811995403388891862u, 12154594426971851389u,         195855442410u },
        {  7051931074990177294u, 15780127219221910901u,         216890213571u },
        {  2030832259446664275u, 16421541930960194380u,          22405811160u },
        {  6069512651054767896u,  7485894627196740575u,         190482321942u },
        { 10608701253763958799u,  8897269432694476706u,         244931862206u },
        { 15700053443426906717u, 17189823634941678804u,         250519635444u },
        { 17759719234725541222u,  9585582064286255215u,          87695812346u },
        { 15187321568916405210u, 12835472279575022096u,         103367328599u },
        { 11040156458113129594u,  6776016669542754607u,         190994214247u },
        {  2800727824598008497u, 18340015775620871026u,         115284830142u },
        {  2997236166375604479u,  5254188752292365829u,         116368563827u },
        {  6260091886451512841u,  6798802596750151182u,          34512248692u },
        { 17573059315228347474u,  9449320530215271999u,              2063650u },
        {  7519453664590169251u,    38067632857031246u,                    0u },
        { 15809436065653866529u,             38067632u,                    0u },
        {                    1u,                    0u,         188927574016u },
        {   228921437623588922u, 17110720482574968810u,         137876709820u },
        {  2195862230003073884u, 16172441693558688212u,           9337981321u },
        {   960207412233973688u,  6234654946353717319u,         101606084361u },
        {  2464387149230492479u, 11180283100679445437u,         143805142629u },
        {  3631866936444955213u, 14852260031176961271u,           7242944399u },
        {  1578304441149380227u,  4481533167346438749u,          48231461895u },
        { 18190538519673445181u,  4269718344362365663u,          59624502064u },
        {  1271000736479934749u, 11520029752381101465u,         112909574203u },
        { 18292963032817745634u, 16778682550309368416u,         132525165168u },
        { 17168014021925537455u,  9687587467301363607u,          21547195268u },
        { 18046757712870378949u, 10093971076828497317u,         175103745301u },
        { 14857998893911743220u,  1913763026490934695u,         147688546991u },
        { 11933607369968684575u, 12701450127613556999u,         250486512531u },
        {  3483798509902859162u,  8974572160711134643u,         137536137978u },
        {  7378828438829845831u,  9890000077336694123u,         143232687497u },
        { 15791137430347699565u,  4292326716201059147u,         173793880975u },
        { 17044141236829932641u, 14644519175104337419u,         254273824941u },
        {  9075651910862456484u,  5051178622270136797u,         229036645118u },
        { 17811207355884564095u,   675983118348065838u,         227240240101u },
        {  4438638126207305937u,  4431647660065117243u,         121450817507u },
        { 12507972635512950185u,  8316115180008411961u,         142521564025u },
        { 14658269128098109408u,  9621158095544965601u,           6828519054u },
        {  3642436268910286111u, 15283478958951102071u,          32757941510u },
        {  3783099432964819561u, 13981553073094447812u,           9247109664u },
        {  9497579866027539638u,  4558368743929911606u,         132824915465u },
        {  3395179445046271361u, 15217004469858477790u,         234628251268u },
        {  5938502732309497276u, 11589190369996515736u,          90198984938u },
        {  5793671185917606255u,  3670624237398152928u,          34730303066u },
        {   889272970253526588u, 13471713758418039776u,         135243399970u },
        {  8594177504370135501u,  4489936967610296410u,                  135u },
        {  7374354721120724712u,        2494800386918u,                    0u },
        { 14764532643665507567u,                 2494u,                    0u },
        {                    1u,                    0u,         117490712576u },
        {  5392404173658087695u,  9052049303222747949u,         112054824309u },
        {  4976586473237854316u,  1011330006193020537u,         133943910512u },
        {  6308932742419013569u, 17412075644359478611u,          40344704645u },
        {  4831846642430703059u,  6358678384745980467u,          29827373864u },
        { 18139507855949846901u, 15262353928842850918u,          49604185629u },
        {  4865833876326628410u, 11145257686438581735u,          65086766641u },
        { 14296661839130179261u,  1600562031807691889u,         223367281473u },
        {  9254773150378118248u,  6775147337046626723u,         217855008735u },
        { 12174712433727875143u, 15772127322106297821u,         113224509657u },
        {   705653145340915199u,  4141472200527441473u,          20989118065u },
        { 17763928858962481812u, 18246007807879281266u,         143052082196u },
        {  3982836567612046296u,   960746958654787122u,          68615608975u },
        { 12730849277561967739u, 11355981212264408476u,         140085276740u },
        { 17314488764367235908u,  1573078209576251480u,          64338558092u },
        { 15951418930590301119u,  6245294478780491366u,         145407838528u },
        {  7193356087283467261u,  7523292955659721509u,          59783592849u },
        { 17592945625696089446u, 14454736751015226504u,          25391385403u },
        {  3554461664875361428u,  7219786377781411315u,          97574471193u },
        {  2213779057785318208u, 10597123082209392430u,         128375261537u },
        {  3880940796082421148u,  6922353544343010713u,         104776154496u },
        {  4528237545358141043u, 14317523356293377429u,         133219971944u },
        { 11681196539088147363u,  4057766168681892716u,          25824757125u },
        {  9835005502912643017u, 15214083611901244044u,           8454853657u },
        {  4964088126040986696u,  8390569016883950720u,          66578989576u },
        {  3355564873147047622u, 10680472538208175054u,          45659930434u },
        {  1853093467828272927u, 12173567833130544926u,         213075153709u },
        { 14755341584803008677u,  1386341248286610025u,         240676937941u },
        {  4701571132542556621u, 12487300952797237351u,         245141746416u },
        {  6128849686644853851u,  2614759871804869719u,          79460481781u },
        { 12026867901170202094u,  8494389567327729476u,              8863311u },
        { 17909760324981426303u,   163499238157084246u,                    0u },
        {  2897692901883393664u,            163499238u,                    0u },
        {                    1u,                    0u,         159339380736u },
        { 12323704802554838154u,  6260469580539185877u,           8965946783u },
        {  7135886931147821732u, 17818573101084525840u,         164119318024u },
        { 15341283120292884947u,  2201029069927307149u,          62563676580u },
        {  3092789040392634166u, 10397997613804897038u,         206773573694u },
        {  8811761390822097865u, 14269915965770103740u,         171909436366u },
        { 16870860798610218169u, 16776139909196366726u,          54338624171u },
        { 17452041453591904833u,  6246513436385199719u,           6158620214u },
        { 10314783684009874908u,  2926026498821554287u,         225852481030u },
        {  4932636630789274903u, 15725499391028340981u,         121464937185u },
        { 18143884346082124480u,  8576577277771450826u,          54841522553u },
        {  2823209155405527322u, 15523351176022259334u,          85258861878u },
        { 16195396106620226251u,  4775158829429176133u,         152549789013u },
        {  1150544491807648944u, 10141817222123532461u,         212696472984u },
        {  7767455475523884824u, 12847658900242624585u,         171743122900u },
        { 15204378045683991808u, 13708197964460514654u,         104105793195u },
        { 17239732561718805622u,  1951540006613246931u,         153540978792u },
        { 12886430624522800062u,  9979297327280092198u,          49833822361u },
        { 18162250541178258136u, 15381307706282553683u,          16544130097u },
        { 17028935366700158084u, 10037428657543061176u,          17140126480u },
        { 16075467823964198637u,  2584877324547208667u,         178061074449u },
        {  9803858825574498304u,  1126624732730703575u,          80081372850u },
        { 17464070808143041817u,  1501064139624981019u,          35282958416u },
        { 17682703471239266776u,  5219661484955306108u,         113289319203u },
        { 18147688354161351336u,  5336997298570282211u,          56660882545u },
        {  6663423873348080051u, 12191131175733833361u,         241200960568u },
        {  9417270363716235133u,  3707068178994436535u,          61273516273u },
        {  9295013721571344179u,  5045484691732942021u,          75804906301u },
        {  6199479138350037783u, 14847900542908711231u,          73493163339u },
        {   887603005365085688u,  9097257915916965134u,         226134008905u },
        {   333989628642975696u,  2472027983230314216u,          68865979874u },
        {  4620735991403939439u, 15974509111133272204u,                  580u },
        { 12418523063962801201u,       10715086071862u,                    0u },
        {  1587745622680169419u,                10715u,                    0u },
        {                    1u,                    0u,         225655914496u },
        { 10968905082284365638u, 12099486841948187398u,          72288392929u },
        { 14076907092801977812u,  5319910566029976327u,         139626084168u },
        {  3438322122816124202u, 11549214421017285863u,          77108354699u },
        { 14645413324829073676u,  1998791413186046699u,           8925646925u },
        { 12271281439492289999u, 17075171930090011209u,         208821732872u },
        {  6233751789862708246u, 15158296003813501473u,         176073730256u },
        {  1962644459455827991u,  1360083178079384114u,         155334366896u },
        {  8726934184642952500u,  6167980558592741157u,          60196792475u },
        {  4531087719737475147u,  3630180428124865652u,           6123412028u },
        {   481513520412720775u,  2276550099763657676u,         110022063878u },
        {   992149349835802669u,   407006713016100654u,          68772091758u },
        { 11165474436676191361u, 14242579061653496001u,         190972772932u },
        { 10240785855143707184u, 17944493332678643703u,          76053515454u },
        { 10059329918238932466u,   987185901870869451u,          61302420044u },
        { 14791716450947031886u,  5578665155415167744u,          21262876221u },
        { 15378882314737417403u,  4849210377429577535u,         125586119445u },
        { 14726970229242271128u, 10811995403388891861u,         117382285949u },
        {  5090110549507128156u,  7051931074990177293u,          76110091637u },
        { 17185220781106503841u,  2030832259446664274u,         223329028940u },
        {  9858517691519529306u,  6069512651054767895u,         162575098847u },
        {  5595905546638020703u, 10608701253763958798u,         212851101602u },
        { 15555173226968030256u, 15700053443426906716u,         111962756308u },
        { 10745236628845355771u, 17759719234725541221u,          16823306351u },
        {  9973314042399760760u, 15187321568916405209u,          47598488080u },
        {  4374506813558796576u, 11040156458113129593u,         114151827759u },
        { 15960826480426749933u,  2800727824598008496u,           5162480498u },
        {  9636454862798615738u,  2997236166375604478u,          14339360261u },
        { 17973331528911319269u,  6260091886451512840u,          63952637454u },
        {  7366495200039369602u, 17573059315228347473u,          78407630399u },
        { 10505831326526933399u,  7519453664590169250u,         176857031246u },
        {  2803218632575724145u, 15809436065653866528u,             38067632u },
        {  8425731874431741636u,   702223880805592151u,                    0u },
        { 14860552245711912111u,            702223880u,                    0u },
        {                    1u,                    0u,         234012409856u },
        {  6993664200669526994u,   228921437623588921u,         212119037930u },
        {  4065363582031999356u,  2195862230003073883u,          71052052948u },
        {  6899780515342669867u,   960207412233973687u,         189133594695u },
        { 17713500890201844939u,  2464387149230492478u,         247196883901u },
        {  6445781125105107086u,  3631866936444955212u,          93085560055u },
        { 13563044070717478571u,  1578304441149380226u,         223986111069u },
        { 13167612994149348885u, 18190538519673445180u,         153068901087u },
        {  5505463469596727288u,  1271000736479934748u,          96991663513u },
        { 12125446212518819372u, 18292963032817745633u,         151930679904u },
        { 12537707724735421794u, 17168014021925537454u,         165978316695u },
        { 15173675086703777069u, 18046757712870378948u,         167805453733u },
        { 13535510174093048476u, 14857998893911743219u,           7646922151u },
        { 10698912997087096629u, 11933607369968684574u,         179188857095u },
        { 16952559548431933861u,  3483798509902859161u,         107400007091u },
        { 13528255827744249993u,  7378828438829845830u,          75856039275u },
        { 14122167436324771955u, 15791137430347699564u,          11923964747u },
        { 13071007137740038297u, 17044141236829932640u,         221491992075u },
        { 13011887609328904025u,  9075651910862456483u,          46965547485u },
        {  3116434332871336590u, 17811207355884564094u,          59240619054u },
        {  9050993820536772770u,  4438638126207305936u,          57678058555u },
        { 11993719123438634238u, 12507972635512950184u,         225794626361u },
        {  1414857165879849301u, 14658269128098109407u,         119197456865u },
        { 13819438220812375094u,  3642436268910286110u,         196205082231u },
        {  6073063033888264440u,  3783099432964819560u,          54514864836u },
        {  6828883869150720294u,  9497579866027539637u,         222184053046u },
        {  4548265621068768345u,  3395179445046271360u,         152321926878u },
        { 10422524923581371874u,  5938502732309497275u,         224314075544u },
        {  1858996082510682634u,  5793671185917606254u,         224048207584u },
        {   890276727450878316u,   889272970253526587u,          90465891296u },
        {  3886008133802710905u,  8594177504370135500u,         102399764570u },
        {   612074409233016757u,  7374354721120724711u,         190800386918u },
        {  3927020336901729264u, 14764532643665507566u,                 2494u },
        {  5298603480094474942u,       46020944252475u,                    0u },
        { 17418383752590430025u,                46020u,                    0u },
        {                    1u,                    0u,          45292322816u },
        {  8973799690601597929u,  5392404173658087694u,         121269781293u },
        {  1343055462055792431u,  4976586473237854315u,          83342007929u },
        { 17425118728683169659u,  6308932742419013568u,          51261934931u },
        { 18389781726026675967u,  4831846642430703058u,         102983344691u },
        {   272526939565961561u, 18139507855949846900u,         231263777382u },
        { 11293026845930963228u,  4865833876326628409u,         113775023591u },
        { 13997416438903902597u, 14296661839130179260u,         163501702257u },
        {  6186605805999441184u,  9254773150378118247u,         221659992483u },
        {  4401776373281836138u, 12174712433727875142u,          65038253533u },
        { 16338917089754547008u,   705653145340915198u,         114962984513u },
        { 13337700757935003056u, 17763928858962481811u,          50215910002u },
        { 14612496890816348693u,  3982836567612046295u,         156690140722u },
        {  3219935399907691719u, 12730849277561967738u,          88938620316u },
        { 10887238730052330387u, 17314488764367235907u,         102864728152u },
        {   360256418697768294u, 15951418930590301118u,          37389952614u },
        {   321440824631118565u,  7193356087283467260u,         136953715493u },
        { 10069228080701402580u, 17592945625696089445u,         243192687752u },
        {  9428069607611622975u,  3554461664875361427u,          46120009203u },
        { 14736799017468812344u,  2213779057785318207u,         153210386222u },
        { 10875332567307979280u,  3880940796082421147u,         149245476249u },
        {  4611492910339012807u,  4528237545358141042u,         108633238933u },
        { 10743508637597314786u, 11681196539088147362u,         140533156716u },
        {  9356196315668016028u,  9835005502912643016u,         128269103756u },
        { 15755598617722189347u,  4964088126040986695u,         206181905536u },
        {  1275276394173375542u,  3355564873147047621u,          30100456398u },
        { 12644999363867216251u,  1853093467828272926u,         105799888670u },
        {  4553830511509832021u, 14755341584803008676u,         103254872681u },
        {  8869400642218174412u,  4701571132542556620u,          87332245607u },
        { 16570849151159054040u,  6128849686644853850u,          68651977815u },
        { 16127119334101797673u, 12026867901170202093u,          86970890052u },
        {  9686867250420930550u, 17909760324981426302u,         230157084246u },
        { 10678226869774428035u,  2897692901883393663u,            163499238u },
        {  7767227962910162068u,  3016028602530220424u,                    0u },
        {  9780840471948993674u,           3016028602u,                    0u },
        {                    1u,                    0u,         213668069376u },
        {  6288709332106746357u, 12323704802554838153u,          16386837205u },
        {  9066785620141948673u,  7135886931147821731u,         141831652624u },
        {  8442375916704414909u, 15341283120292884946u,          14167660429u },
        { 11604629218100425803u,  3092789040392634165u,         188477686542u },
        {  3877248044010875762u,  8811761390822097864u,         134914571196u },
        { 16435137704395217283u, 16870860798610218168u,         103946077062u },
        { 14994442577577813271u, 17452041453591904832u,         111559165543u },
        {  4410105917142436089u, 10314783684009874907u,         245267398767u },
        {  4632574728444936970u,  4932636630789274902u,         202983581941u },
        {  9117147535650050359u, 18143884346082124479u,         134153046474u },
        {   588939301256904809u,  2823209155405527321u,          69877954182u },
        {   324393982565305683u, 16195396106620226250u,         173062371141u },
        {  9380909186923521175u,  1150544491807648943u,          73421074605u },
        {  4463385697777230217u,  7767455475523884823u,          94824230985u },
        { 16378985502426333808u, 15204378045683991807u,         211934567774u },
        { 18210894922387834354u, 17239732561718805621u,          38698574803u },
        {  1555748035329493205u, 12886430624522800061u,          83984577574u },
        {  4277055533891898507u, 18162250541178258135u,         184923140435u },
        { 11574429772510874408u, 17028935366700158083u,         219871452856u },
        { 17391099253493808815u, 16075467823964198636u,         215531468251u },
        {  5791212393959129882u,  9803858825574498303u,          27946729175u },
        { 11254268231455680880u, 17464070808143041816u,         124958581275u },
        { 16355477587312235322u, 17682703471239266775u,         227983788156u },
        {  2411485149249320633u, 18147688354161351335u,         145361224931u },
        { 12763114642070638360u,  6663423873348080050u,         183510511249u },
        {  1147543073987366419u,  9417270363716235132u,         197503883703u },
        {  8410777835225272692u,  9295013721571344178u,          63336074437u },
        {  8134725822306818018u,  6199479138350037782u,          14048117055u },
        {  8899607004752328377u,   887603005365085687u,         232018105614u },
        {   690976506652396830u,   333989628642975695u,         140250490600u },
        { 12281570945595192074u,  4620735991403939438u,          54673209484u },
        { 12592957291365552899u, 12418523063962801200u,         219086071862u },
        { 13595807339013970272u,  1587745622680169418u,                10715u },
        {  9698096389749839992u,      197658450495420u,                    0u },
        {  8310173728816391804u,               197658u,                    0u },
    };

    static constexpr u32 TABLE_SIZE_2 = 69;
    static constexpr u32 ADDITIONAL_BITS_2 = 120;

    static constexpr u16 POW10_OFFSET_2[TABLE_SIZE_2] = {
            0,    2,    6,   12,   20,   29,   40,   52,   66,   80,
            95,  112,  130,  150,  170,  192,  215,  240,  265,  292,
        320,  350,  381,  413,  446,  480,  516,  552,  590,  629,
        670,  712,  755,  799,  845,  892,  940,  989, 1040, 1092,
        1145, 1199, 1254, 1311, 1369, 1428, 1488, 1550, 1613, 1678,
        1743, 1810, 1878, 1947, 2017, 2088, 2161, 2235, 2311, 2387,
        2465, 2544, 2625, 2706, 2789, 2873, 2959, 3046, 3133
    };

    static constexpr u8 MIN_BLOCK_2[TABLE_SIZE_2] = {
        0,    0,    0,    0,    0,    0,    1,    1,    2,    3,
        3,    4,    4,    5,    5,    6,    6,    7,    7,    8,
        8,    9,    9,   10,   11,   11,   12,   12,   13,   13,
        14,   14,   15,   15,   16,   16,   17,   17,   18,   19,
        19,   20,   20,   21,   21,   22,   22,   23,   23,   24,
        24,   25,   26,   26,   27,   27,   28,   28,   29,   29,
        30,   30,   31,   31,   32,   32,   33,   34,    0
    };

    static constexpr u64 POW10_SPLIT_2[3133][3] = {
        {                    0u,                    0u,              3906250u },
        {                    0u,                    0u,         202000000000u },
        {                    0u, 11153727427136454656u,                   59u },
        {                    0u,  7205759403792793600u,          59604644775u },
        {                    0u,                    0u,         167390625000u },
        {                    0u,                    0u,         232000000000u },
        {                    0u,    16777216000000000u,                    0u },
        {                    0u, 12945425605062557696u,               909494u },
        {                    0u,  4388757836872548352u,         182701772928u },
        {                    0u,  1152921504606846976u,         128237915039u },
        {                    0u,                    0u,         159062500000u },
        {                    0u,                    0u,         160000000000u },
        {                    0u,         256000000000u,                    0u },
        {                    0u, 16192327041775828992u,                   13u },
        {                    0u, 15024075324038053888u,          13877787807u },
        {                    0u,  5449091666327633920u,         159814456755u },
        {                    0u,  2494994193563254784u,         179295395851u },
        {                    0u,  4611686018427387904u,          11135253906u },
        {                    0u,                    0u,         146250000000u },
        {                    0u,                    0u,         128000000000u },
        {                    0u,              3906250u,                    0u },
        {                    0u,     3906250000000000u,                    0u },
        {                    0u,  4368439412768899072u,               211758u },
        {                    0u,  1563676642168012800u,          46236813575u },
        {                    0u, 11532349341402398720u,           7084767080u },
        {                    0u,  9048364970084925440u,         104625169910u },
        {                    0u, 16609275425742389248u,         246490512847u },
        {                    0u,                    0u,         207900390625u },
        {                    0u,                    0u,         225000000000u },
        { 11153727427136454656u,                   59u,                    0u },
        {  7205759403792793600u,          59604644775u,                    0u },
        {                    0u,  4264412554261970152u,                    3u },
        {                    0u, 14485570586272534528u,           3231174267u },
        {                    0u, 17827675094632103936u,         123785264354u },
        {                    0u,  7347197909193981952u,         226966440203u },
        {                    0u, 13677404030777688064u,          11398292396u },
        {                    0u,  3810326759732150272u,         172741453558u },
        {                    0u,  9943947977234055168u,         246206558227u },
        {                    0u,                    0u,          19539062500u },
        {                    0u,                    0u,         228000000000u },
        { 12945425605062557696u,               909494u,                    0u },
        {  4388757836872548352u,      909494701772928u,                    0u },
        {  1152921504606846976u, 14878706826214591391u,                49303u },
        {                    0u,  4387341015746028192u,         151806576313u },
        {                    0u,   651726680428265472u,         185237838233u },
        {                    0u,  2570638187944738816u,         153035330174u },
        {                    0u,  7419175577111756800u,         126139354575u },
        {                    0u, 17299322326264840192u,         207402194313u },
        {                    0u,  7990511638862102528u,         137937798142u },
        {                    0u, 16717361816799281152u,         254433166503u },
        {                    0u,                    0u,         167906250000u },
        {                    0u,                    0u,          16000000000u },
        { 16192327041775828992u,                   13u,                    0u },
        { 15024075324038053888u,          13877787807u,                    0u },
        {  5449091666327633920u, 13877787807814456755u,                    0u },
        {  2494994193563254784u,  9707857417284919307u,            752316384u },
        {  4611686018427387904u,  1844515466944871826u,         224526264005u },
        {                    0u, 15167599819856275072u,         197099991383u },
        {                    0u, 14830185305589481472u,          87822237233u },
        {                    0u,  6163721531743535104u,          49803945956u },
        {                    0u, 14122847407012052992u,         228334136013u },
        {                    0u,   335491783960035328u,         205765601092u },
        {                    0u,   941252322120433664u,          68018187046u },
        {                    0u, 11529215046068469760u,          38051025390u },
        {                    0u,                    0u,         238625000000u },
        {                    0u,                    0u,          64000000000u },
        {  4368439412768899072u,               211758u,                    0u },
        {  1563676642168012800u,      211758236813575u,                    0u },
        { 11532349341402398720u,  8061591463141767016u,                11479u },
        {  9048364970084925440u, 16628725344207857142u,         215437019748u },
        { 16609275425742389248u,  3555541870038531535u,         100901445007u },
        {                    0u, 18316647450161853665u,         143192746310u },
        {                    0u, 16709574568378075648u,          70992947447u },
        {                    0u,  7696022835795591168u,         247905827852u },
        {                    0u, 16664449640376041472u,          12417202233u },
        {                    0u,  3109186955116544000u,          57903381625u },
        {                    0u, 10515518101817131008u,         121168549362u },
        {                    0u,  9961962375743537152u,         242570047378u },
        {                    0u,  9223372036854775808u,         146540039062u },
        {                    0u,                    0u,         150500000000u },
        { 14485570586272534528u,           3231174267u,                    0u },
        { 17827675094632103936u,  3231174267785264354u,                    0u },
        {  7347197909193981952u,   748977172262750475u,            175162308u },
        { 13677404030777688064u, 15965033457315095468u,         196040602133u },
        {  3810326759732150272u, 16809402149066729206u,          21865466197u },
        {  9943947977234055168u,  7563769067065700371u,          85911239516u },
        {                    0u, 13550322810840051428u,          92410032742u },
        {                    0u,  8663209637545764864u,         102734564471u },
        {                    0u,  8969247575312957440u,         119469633535u },
        {                    0u,  6193172891660451840u,         255486223885u },
        {                    0u,  3427954273864908800u,          13335732575u },
        {                    0u, 10058367555266936832u,          95185829773u },
        {                    0u, 13907115649320091648u,         141545265197u },
        {                    0u,                    0u,          45753906250u },
        {                    0u,                    0u,          74000000000u },
        { 14878706826214591391u,                49303u,                    0u },
        {  4387341015746028192u,       49303806576313u,                    0u },
        {   651726680428265472u, 14106411361315920281u,                 2672u },
        {  2570638187944738816u,  3609034283485221502u,         112764710092u },
        {  7419175577111756800u,  9896072247338192335u,         204195646140u },
        { 17299322326264840192u,  8889095178479228297u,         188536467151u },
        {  7990511638862102528u,  3631796911038383102u,         207481878815u },
        { 16717361816799281152u,   898318840772166823u,          31196880105u },
        {                    0u, 17293677953982795024u,         233048697961u },
        {                    0u,  7353628266884669440u,         105937492160u },
        {                    0u,  2404693032470315008u,         192398640987u },
        {                    0u,  9191155893041889280u,          91130358670u },
        {                    0u,  6353946855033798656u,         142498253559u },
        {                    0u,  3767824038248841216u,         247344448149u },
        {                    0u,  7205759403792793600u,         149204254150u },
        {                    0u,                    0u,         198390625000u },
        {                    0u,                    0u,         232000000000u },
        {  9707857417284919307u,            752316384u,                    0u },
        {  1844515466944871826u,   752316384526264005u,                    0u },
        { 15167599819856275072u, 17063068157692817751u,             40783152u },
        { 14830185305589481472u,  5385330256507239985u,          48924990778u },
        {  6163721531743535104u,  3373050282752075748u,          58291939338u },
        { 14122847407012052992u,  4116064001262906061u,          10182853422u },
        {   335491783960035328u, 11306582046748043076u,          46223132276u },
        {   941252322120433664u, 17035410946089626406u,         116612931040u },
        { 11529215046068469760u, 15618595715183448558u,         224923491477u },
        {                    0u,  5141740092277295680u,         149846685770u },
        {                    0u, 16973644291514990592u,          74278734288u },
        {                    0u, 14625255268443750400u,         208920143100u },
        {                    0u, 14021170507320131584u,         252792836676u },
        {                    0u,  4451355232865091584u,          68760089176u },
        {                    0u, 12891553933348044800u,          88241308450u },
        {                    0u,  1152921504606846976u,          34698852539u },
        {                    0u,                    0u,         187062500000u },
        {                    0u,                    0u,         160000000000u },
        {  8061591463141767016u,                11479u,                    0u },
        { 16628725344207857142u,       11479437019748u,                    0u },
        {  3555541870038531535u,  5562205901560339855u,                  622u },
        { 18316647450161853665u,  2106077949367544134u,         110301527786u },
        { 16709574568378075648u,  7496855998374373623u,         234114170714u },
        {  7696022835795591168u,   229183437194837004u,          90406405378u },
        { 16664449640376041472u,   465169186276472889u,           2012424059u },
        {  3109186955116544000u,  2152980561625316473u,         123025216872u },
        { 10515518101817131008u,  2059790725449340402u,         104116713310u },
        {  9961962375743537152u, 17891190926410198930u,          94111661478u },
        {  9223372036854775808u,  9930696175609809814u,         166969883403u },
        {                    0u,  7276914261609005312u,          11538344118u },
        {                    0u, 10539762974036983808u,         182394482312u },
        {                    0u, 12851089458992250880u,         136571361695u },
        {                    0u,  9449311677678878720u,         159696658955u },
        {                    0u,  8699564697382289408u,          11512248212u },
        {                    0u,  4224376450473525248u,         148471604347u },
        {                    0u,  4611686018427387904u,         123229003906u },
        {                    0u,                    0u,         130250000000u },
        {                    0u,                    0u,         128000000000u },
        {   748977172262750475u,            175162308u,                    0u },
        { 15965033457315095468u,   175162308040602133u,                    0u },
        { 16809402149066729206u, 13756840147955779925u,              9495567u },
        {  7563769067065700371u, 13788447602092505948u,          15745759798u },
        { 13550322810840051428u,  4972540435632173670u,          54747473242u },
        {  8663209637545764864u,  2844874687533091959u,          90269561957u },
        {  8969247575312957440u, 15377573779532804095u,         101154220965u },
        {  6193172891660451840u, 17824715805091194381u,         165833619944u },
        {  3427954273864908800u, 18277569135638159711u,         232966279779u },
        { 10058367555266936832u,  4254645803379752845u,          99990829008u },
        { 13907115649320091648u,  2933643244178200621u,         208230644811u },
        {                    0u, 17188148801879487562u,          75159033118u },
        {                    0u, 11069762501163246592u,          30931771413u },
        {                    0u, 11676570643941818368u,          21600093027u },
        {                    0u, 17840016768744030208u,          99632988162u },
        {                    0u, 16463817321652158464u,           2967109246u },
        {                    0u,  6954191143357644800u,         126892505325u },
        {                    0u,  5080060379673919488u,         237376987457u },
        {                    0u,                    0u,          65275390625u },
        {                    0u,                    0u,         161000000000u },
        { 14106411361315920281u,                 2672u,                    0u },
        {  3609034283485221502u,        2672764710092u,                    0u },
        {  9896072247338192335u, 16433563478020213436u,                  144u },
        {  8889095178479228297u,  4194750497955655375u,         144890865261u },
        {  3631796911038383102u,  2691539602252904735u,         109227397880u },
        {   898318840772166823u,  3775467271962795241u,         248145908654u },
        { 17293677953982795024u, 16980212613224918121u,         174204668490u },
        {  7353628266884669440u,  4172857038337333440u,          74920499170u },
        {  2404693032470315008u,  5936867627376461659u,         226226211033u },
        {  9191155893041889280u, 17856837443266866062u,         217321838238u },
        {  6353946855033798656u,  8956297047799810807u,         158968021097u },
        {  3767824038248841216u, 15356974049716912789u,         105485521835u },
        {  7205759403792793600u,  6923608913322982854u,         171832503231u },
        {                    0u,  4855902993563955944u,         191375329591u },
        {                    0u, 13835893222288330752u,          55263239028u },
        {                    0u,  9114973913760137216u,         116750045274u },
        {                    0u, 17937099003422310400u,          90494123725u },
        {                    0u,  7007960010734960640u,         205972372085u },
        {                    0u,  7683422439270776832u,         117379902273u },
        {                    0u,   720575940379279360u,          65416519165u },
        {                    0u,                    0u,         253039062500u },
        {                    0u,                    0u,         228000000000u },
        { 17063068157692817751u,             40783152u,                    0u },
        {  5385330256507239985u,    40783152924990778u,                    0u },
        {  3373050282752075748u,  2768933352715741194u,              2210859u },
        {  4116064001262906061u, 15201941611824153390u,          43150104177u },
        { 11306582046748043076u,  1418128541727000180u,         113824098906u },
        { 17035410946089626406u,  5353350204565757408u,          90076876902u },
        { 15618595715183448558u,  1721001680354286741u,         102290205696u },
        {  5141740092277295680u,   637631411660453962u,             93295688u },
        { 16973644291514990592u,  1630012588870568400u,          72034566068u },
        { 14625255268443750400u,  9253063571656828156u,         180088363159u },
        { 14021170507320131584u,  6029146854993203780u,         151501609581u },
        {  4451355232865091584u, 16987401965352759896u,         109326840705u },
        { 12891553933348044800u, 14499131620542087970u,         129920888905u },
        {  1152921504606846976u,  1978417255298660539u,          73785999500u },
        {                    0u,  5790079354402454176u,         140107250214u },
        {                    0u, 13748918935842078720u,          38313880830u },
        {                    0u, 18047438014740692992u,         254745330388u },
        {                    0u,  3116889656839372800u,         212978353575u },
        {                    0u, 15995952446606147584u,         167168966926u },
        {                    0u, 12530140063251562496u,          14867142319u },
        {                    0u, 16717361816799281152u,         175679260253u },
        {                    0u,                    0u,          93906250000u },
        {                    0u,                    0u,          16000000000u },
        {  5562205901560339855u,                  622u,                    0u },
        {  2106077949367544134u,         622301527786u,                    0u },
        {  7496855998374373623u, 13558973353698967386u,                   33u },
        {   229183437194837004u,  6228991722850501890u,          33735033418u },
        {   465169186276472889u, 16886831391703377787u,          74337674317u },
        {  2152980561625316473u,  1181713637872883048u,          77915436964u },
        {  2059790725449340402u, 12393932434925221726u,         164064060824u },
        { 17891190926410198930u, 10684799845419711910u,         152671876423u },
        {  9930696175609809814u,  4590318792215640843u,          71579224160u },
        {  7276914261609005312u,  6383712187366189238u,          96248841680u },
        { 10539762974036983808u,  1904270214927675016u,         208346061731u },
        { 12851089458992250880u,  3711506775113308575u,         163103230695u },
        {  9449311677678878720u,  8091219444738793995u,         231201201185u },
        {  8699564697382289408u,    39436684991068052u,          33438625885u },
        {  4224376450473525248u, 18025182908196512891u,          93002137866u },
        {  4611686018427387904u,  7853924592034603138u,          10977147123u },
        {                    0u,  4815749283615688320u,         243425762105u },
        {                    0u, 14242399906544287744u,          57261062291u },
        {                    0u,    76242322576113664u,         147772082046u },
        {                    0u, 10858088421377703936u,         126004133104u },
        {                    0u, 14293835879041466368u,         240588618152u },
        {                    0u, 12182236992037191680u,         168774870395u },
        {                    0u, 11529215046068469760u,         123660400390u },
        {                    0u,                    0u,           6625000000u },
        {                    0u,                    0u,          64000000000u },
        { 13756840147955779925u,              9495567u,                    0u },
        { 13788447602092505948u,     9495567745759798u,                    0u },
        {  4972540435632173670u, 14000097438505379162u,               514755u },
        {  2844874687533091959u, 16451062686452429925u,         195758946802u },
        { 15377573779532804095u,  4009347599785716645u,         242891813895u },
        { 17824715805091194381u, 16544162347546196456u,           7217347168u },
        { 18277569135638159711u, 17674258299745817187u,          96896860837u },
        {  4254645803379752845u,  5215238411201214416u,         165958123462u },
        {  2933643244178200621u, 14253990228345322571u,         198282718640u },
        { 17188148801879487562u, 11214836553940194590u,         176772710358u },
        { 11069762501163246592u, 14620711348380590101u,         214607957507u },
        { 11676570643941818368u,  6638710787931587427u,           3792590350u },
        { 17840016768744030208u, 17320000343692853250u,          14359885232u },
        { 16463817321652158464u,    75147386268843646u,         176938919100u },
        {  6954191143357644800u, 17938801582125480173u,         188004073747u },
        {  5080060379673919488u,  6573358613626446145u,          19972464382u },
        {                    0u,  8688505427903736481u,         254356342484u },
        {                    0u,   539870168696556032u,         212471004823u },
        {                    0u,  9002861336394465280u,         151029266420u },
        {                    0u, 17989846818158018560u,         244488046090u },
        {                    0u,  2700938287723315200u,          10975231550u },
        {                    0u, 17800090499088908288u,          62146418157u },
        {                    0u,  8809040871136690176u,         237964944839u },
        {                    0u,  9223372036854775808u,         199477539062u },
        {                    0u,                    0u,         246500000000u },
        { 16433563478020213436u,                  144u,                    0u },
        {  4194750497955655375u,         144890865261u,                    0u },
        {  2691539602252904735u, 15763656745260536568u,                    7u },
        {  3775467271962795241u,  8787336846248645550u,           7854549544u },
        { 16980212613224918121u, 17584084447880694346u,          40476362484u },
        {  4172857038337333440u, 18041672551129683938u,         244953235127u },
        {  5936867627376461659u, 14025886302294509785u,         183978041028u },
        { 17856837443266866062u, 18430498103283160734u,         196760344819u },
        {  8956297047799810807u,  3292348826238025833u,         243999119304u },
        { 15356974049716912789u,  9211721212658275243u,         200178478587u },
        {  6923608913322982854u, 10233245872666307519u,         251499368407u },
        {  4855902993563955944u,  6200995035623311671u,         215554745370u },
        { 13835893222288330752u,  8480542380570450804u,          26336156614u },
        {  9114973913760137216u, 11870363864499900506u,         198459731123u },
        { 17937099003422310400u,  9301051379839581901u,         179643493714u },
        {  7007960010734960640u, 11456694803569638005u,          82504211005u },
        {  7683422439270776832u, 14327208890643983169u,          61621068669u },
        {   720575940379279360u,  4510081789599866365u,         125776679550u },
        {                    0u, 13255356976020303332u,         126244492023u },
        {                    0u,  9658806854127314944u,         247718574341u },
        {                    0u, 13708435528809971712u,           5523604968u },
        {                    0u,  1580190652103131136u,         232743135779u },
        {                    0u, 16557336970347413504u,          35085662306u },
        {                    0u, 12751520132434493440u,          98897575035u },
        {                    0u,  9295429630892703744u,         123691261291u },
        {                    0u,                    0u,         107503906250u },
        {                    0u,                    0u,         202000000000u },
        {  2768933352715741194u,              2210859u,                    0u },
        { 15201941611824153390u,     2210859150104177u,                    0u },
        {  1418128541727000180u, 16872870088062921306u,               119850u },
        {  5353350204565757408u,  5112979788807802982u,          42914680120u },
        {  1721001680354286741u, 13742728082020150272u,          56277175189u },
        {   637631411660453962u,  2217110934613627976u,         149744994782u },
        {  1630012588870568400u, 11021433940188610484u,         222120189824u },
        {  9253063571656828156u,  1713669895470733463u,         128597473131u },
        {  6029146854993203780u,  3313382510572018285u,         107092898231u },
        { 16987401965352759896u, 14976595232784069505u,         183179618825u },
        { 14499131620542087970u,  7213172372862496841u,           9811882854u },
        {  1978417255298660539u, 15836474542502248588u,         102391026857u },
        {  5790079354402454176u,  3221099285878340134u,         169858497005u },
        { 13748918935842078720u,  3265814602578095358u,         237174616142u },
        { 18047438014740692992u,  6502528252282225364u,          78177040164u },
        {  3116889656839372800u, 16392476834556790183u,          36352502762u },
        { 15995952446606147584u, 15167629413417091342u,         234888637949u },
        { 12530140063251562496u,  1366763272626280111u,         253822238838u },
        { 16717361816799281152u,  8720523635169216093u,         118074092385u },
        {                    0u,  9649171375767398672u,          97472740533u },
        {                    0u,  7647980704001073152u,         181523082628u },
        {                    0u, 13286434495608651776u,         132414597864u },
        {                    0u,  4358271637167013888u,         232720259057u },
        {                    0u, 15954987941890097152u,         241236262378u },
        {                    0u,  7911135695429697536u,         234864921629u },
        {                    0u,  7205759403792793600u,          29428863525u },
        {                    0u,                    0u,          37390625000u },
        {                    0u,                    0u,         232000000000u },
        { 13558973353698967386u,                   33u,                    0u },
        {  6228991722850501890u,          33735033418u,                    0u },
        { 16886831391703377787u, 15288289344628122701u,                    1u },
        {  1181713637872883048u,   952589339068938148u,           1828779826u },
        { 12393932434925221726u, 10058155040190817688u,          50051639971u },
        { 10684799845419711910u,  5322725640026584391u,         163545253677u },
        {  4590318792215640843u,  2269982385930389600u,          45288545535u },
        {  6383712187366189238u, 13216683679976310224u,         255123055991u },
        {  1904270214927675016u, 17417440642083494819u,         119716477857u },
        {  3711506775113308575u,  3029180749090900711u,         161944201349u },
        {  8091219444738793995u,  8315443826261908513u,         133164212217u },
        {    39436684991068052u,  1488962797247197277u,         249450781113u },
        { 18025182908196512891u, 18009099634999034122u,         185080716834u },
        {  7853924592034603138u,  8092455412807497971u,          34976275247u },
        {  4815749283615688320u, 17808458047236758329u,          47438692886u },
        { 14242399906544287744u,  3164591817527425171u,          22965398445u },
        {    76242322576113664u,  3314036340472350590u,         173171552866u },
        { 10858088421377703936u,    33234902404332784u,          98179654270u },
        { 14293835879041466368u, 12349284717857274280u,         126001801667u },
        { 12182236992037191680u, 18209607903013119355u,         195669456065u },
        { 11529215046068469760u,  7891549145984268038u,         193987144822u },
        {                    0u,  7703609897518594624u,         118427801736u },
        {                    0u,  6336912652634587136u,         136417613529u },
        {                    0u,  4461621834659397632u,         217343524723u },
        {                    0u,  5484660635557953536u,         115241865004u },
        {                    0u, 15142619273265938432u,          44297324048u },
        {                    0u, 12170977992968765440u,          16820883035u },
        {                    0u,  1152921504606846976u,          91659790039u },
        {                    0u,                    0u,         215062500000u },
        {                    0u,                    0u,         160000000000u },
        { 14000097438505379162u,               514755u,                    0u },
        { 16451062686452429925u,      514755758946802u,                    0u },
        {  4009347599785716645u, 17812314011563521031u,                27904u },
        { 16544162347546196456u,  7684138864490314336u,            965607477u },
        { 17674258299745817187u,  9740522787420029605u,          53416558002u },
        {  5215238411201214416u,  6701109407732989894u,         178528034798u },
        { 14253990228345322571u, 16534886227502443952u,         238363267868u },
        { 11214836553940194590u,  8908667306968317910u,          28896357978u },
        { 14620711348380590101u,  7531472173477105155u,          90482939822u },
        {  6638710787931587427u, 11527371604834801166u,         174408281924u },
        { 17320000343692853250u, 15688593496691078576u,          68624900066u },
        {    75147386268843646u, 11394944804253312188u,         226850480357u },
        { 17938801582125480173u, 11182279880854372627u,         229617721195u },
        {  6573358613626446145u,   150579373068361470u,         107606192607u },
        {  8688505427903736481u,  3147220002440857300u,         223008162924u },
        {   539870168696556032u,  3630514817795505815u,         108170611138u },
        {  9002861336394465280u, 11708796588334233588u,         194196810602u },
        { 17989846818158018560u, 16844495466426369546u,         106634735134u },
        {  2700938287723315200u, 17636655472325475902u,          30913141928u },
        { 17800090499088908288u, 17038926655686645229u,         168956085008u },
        {  8809040871136690176u, 15602838456783529415u,          16923682064u },
        {  9223372036854775808u, 10869815869248876790u,          16845831567u },
        {                    0u, 18407124180939800832u,         143589253898u },
        {                    0u,  5705018517251293184u,          10997852201u },
        {                    0u,  9660452258743058432u,          41309269673u },
        {                    0u,  5646292272224927744u,         169523694166u },
        {                    0u,  7410409304047484928u,          86306086117u },
        {                    0u,  5953758707383795712u,         229401719093u },
        {                    0u,  4611686018427387904u,          53322753906u },
        {                    0u,                    0u,         114250000000u },
        {                    0u,                    0u,         128000000000u },
        { 15763656745260536568u,                    7u,                    0u },
        {  8787336846248645550u,           7854549544u,                    0u },
        { 17584084447880694346u,  7854549544476362484u,                    0u },
        { 18041672551129683938u,    15035424419724983u,            425795984u },
        { 14025886302294509785u, 18280822466032836292u,         144000815071u },
        { 18430498103283160734u, 11524250747302615283u,         223991005371u },
        {  3292348826238025833u, 15212285943691810760u,         187624730884u },
        {  9211721212658275243u,  7951804027551297019u,           4824659673u },
        { 10233245872666307519u,  1706416229965221847u,         217431068160u },
        {  6200995035623311671u,  3406023111930700826u,             92505009u },
        {  8480542380570450804u, 16132696204133391302u,         177184640882u },
        { 11870363864499900506u, 11593846688794356915u,         114874555213u },
        {  9301051379839581901u,  6875759884161133906u,          77628503688u },
        { 11456694803569638005u,  3593593325323835965u,         136372735690u },
        { 14327208890643983169u,  9542049733257388925u,         202194809084u },
        {  4510081789599866365u,  9926551925937787518u,         252517275552u },
        { 13255356976020303332u,  3128491553219547895u,         160538119458u },
        {  9658806854127314944u, 17158408656931354885u,          34169595866u },
        { 13708435528809971712u,  2065169543154992616u,         218930159197u },
        {  1580190652103131136u,  4832622393556232739u,          93111953065u },
        { 16557336970347413504u, 16505930714733656162u,         169261976984u },
        { 12751520132434493440u, 18270988073492888699u,         152894788296u },
        {  9295429630892703744u,  2525111411519708523u,         200990472248u },
        {                    0u, 16728989342518570442u,          56136886563u },
        {                    0u,  7974052022039438336u,          35906880329u },
        {                    0u,  5356554962386550784u,          73432274226u },
        {                    0u,  6693869495028547584u,          50290379426u },
        {                    0u,  8157517147199766528u,         162362875392u },
        {                    0u, 12065776720423157760u,            442219890u },
        {                    0u, 11997589407315001344u,         114654087066u },
        {                    0u,                    0u,         154650390625u },
        {                    0u,                    0u,          97000000000u },
        { 16872870088062921306u,               119850u,                    0u },
        {  5112979788807802982u,      119850914680120u,                    0u },
        { 13742728082020150272u,  2418433229320326037u,                 6497u },
        {  2217110934613627976u,  1143911773589293534u,          97131103528u },
        { 11021433940188610484u,  9276183703610924928u,          40062011581u },
        {  1713669895470733463u,  3532180128827684715u,         189502862926u },
        {  3313382510572018285u,  8563997501322031543u,          78191479868u },
        { 14976595232784069505u, 14843890409658460681u,          60464255234u },
        {  7213172372862496841u,  9489417861634552678u,           2804688911u },
        { 15836474542502248588u,  1113198223322322089u,          15514422373u },
        {  3221099285878340134u, 11190777557146597869u,         101060346596u },
        {  3265814602578095358u, 17764553645932638286u,         228606653266u },
        {  6502528252282225364u, 14900777150991234852u,          82963018382u },
        { 16392476834556790183u, 17364899863357893610u,         142807772747u },
        { 15167629413417091342u, 15537570181590167037u,          75941353107u },
        {  1366763272626280111u,  5558052627121307766u,         147842293367u },
        {  8720523635169216093u, 12095241565795232609u,         119301302636u },
        {  9649171375767398672u,  2187936505958366389u,         108655684359u },
        {  7647980704001073152u, 12009203621325860228u,           7118608275u },
        { 13286434495608651776u, 14814842834750302952u,         147651020232u },
        {  4358271637167013888u,  5965296499605198833u,         200803114239u },
        { 15954987941890097152u,  4051026394962148842u,         255323379371u },
        {  7911135695429697536u, 16799526299141688349u,         171219606580u },
        {  7205759403792793600u,  9460214166646215205u,          52910704145u },
        {                    0u, 10750736995029068008u,          17512839237u },
        {                    0u,  5377963045376430080u,          69582798620u },
        {                    0u, 15996910350253424640u,          28291539960u },
        {                    0u, 13651157529655246848u,         248867194247u },
        {                    0u,  9771305410219737088u,         135740030732u },
        {                    0u, 12709439623416250368u,          12529703527u },
        {                    0u,  9943947977234055168u,         103688980102u },
        {                    0u,                    0u,         134539062500u },
        {                    0u,                    0u,         228000000000u },
        {   952589339068938148u,           1828779826u,                    0u },
        { 10058155040190817688u,  1828779826051639971u,                    0u },
        {  5322725640026584391u,   371564423966525229u,             99138353u },
        {  2269982385930389600u, 14464859121514339583u,          49020142547u },
        { 13216683679976310224u,  3913119023023056247u,         211784141584u },
        { 17417440642083494819u,  5493396321716566945u,          16212130607u },
        {  3029180749090900711u,  5837454566818211973u,          47297797611u },
        {  8315443826261908513u,  2886670683193253881u,         235316449046u },
        {  1488962797247197277u,  5504823105587173817u,          22156486731u },
        { 18009099634999034122u,  9431834277334851106u,          75298417058u },
        {  8092455412807497971u, 12921661346456247087u,         162511300760u },
        { 17808458047236758329u,  3643076516404724246u,         152700484665u },
        {  3164591817527425171u, 12559396953196866477u,          57197491573u },
        {  3314036340472350590u,  1626880974916825698u,         117680846273u },
        {    33234902404332784u,  6806994170946429566u,         193088193394u },
        { 12349284717857274280u,  7596631230206896579u,         114369007893u },
        { 18209607903013119355u,  3100480253729502401u,          21411814204u },
        {  7891549145984268038u,  6310570748781063286u,          60168077371u },
        {  7703609897518594624u, 14251867077375744136u,          59342096725u },
        {  6336912652634587136u,  6701165793751570137u,          85772595262u },
        {  4461621834659397632u, 10856833140463959923u,          62363270925u },
        {  5484660635557953536u, 15867563727561248556u,          13588550103u },
        { 15142619273265938432u,  5048961008671491600u,         215860182353u },
        { 12170977992968765440u, 13278183119599849051u,          81273704724u },
        {  1152921504606846976u,  4547591784941053655u,          20719811749u },
        {                    0u, 11815437715887182496u,         165246525444u },
        {                    0u,   398495392178782208u,           4640516162u },
        {                    0u,  9154841240825495552u,          66021602478u },
        {                    0u,  1902683298245640192u,         174496284938u },
        {                    0u,  5081900962138816512u,          10103144668u },
        {                    0u,  3234710432358858752u,         220275490403u },
        {                    0u, 16717361816799281152u,          99175354003u },
        {                    0u,                    0u,         147906250000u },
        {                    0u,                    0u,          16000000000u },
        { 17812314011563521031u,                27904u,                    0u },
        {  7684138864490314336u,       27904965607477u,                    0u },
        {  9740522787420029605u, 13488568028574514610u,                 1512u },
        {  6701109407732989894u,   275784718433886190u,         232731216738u },
        { 16534886227502443952u, 10020568880357102364u,          98014950319u },
        {  8908667306968317910u,  8876397213146246746u,         175543216127u },
        {  7531472173477105155u,  2155905919114811310u,         255481190457u },
        { 11527371604834801166u,  1087100407155601220u,          57116871894u },
        { 15688593496691078576u,  2903498381705011170u,         214058931831u },
        { 11394944804253312188u, 12223476257006657765u,         119157398962u },
        { 11182279880854372627u, 12148657163736735595u,         178662635975u },
        {   150579373068361470u,  8951241323311673823u,         199658580024u },
        {  3147220002440857300u,  8463862715901576300u,          56485247764u },
        {  3630514817795505815u,  3873401978748963266u,          20458826917u },
        { 11708796588334233588u,   248364795947002730u,         165209977542u },
        { 16844495466426369546u, 10454378025404001822u,         198013463882u },
        { 17636655472325475902u,  6574176865628265640u,          74566732968u },
        { 17038926655686645229u,    16703315293848336u,         168356386842u },
        { 15602838456783529415u,  9896033222450013456u,          26000905488u },
        { 10869815869248876790u, 17311376269334085007u,          16536465035u },
        { 18407124180939800832u, 18378511316495639306u,         139938451587u },
        {  5705018517251293184u, 15120796393727584297u,         131996301094u },
        {  9660452258743058432u, 18253447805740347049u,          38819700014u },
        {  5646292272224927744u,  5842497225601731158u,          46989521388u },
        {  7410409304047484928u,  4369968404176723173u,         236316722409u },
        {  5953758707383795712u, 16142207253674488117u,         233236896461u },
        {  4611686018427387904u, 12124259227391928178u,         205875070808u },
        {                    0u, 13019483264566077056u,          88657257409u },
        {                    0u,    74901376448135168u,         193705787602u },
        {                    0u, 13897060093813325824u,         210004060411u },
        {                    0u,  4495486210810052608u,         251753361137u },
        {                    0u, 14885496280087265280u,         241243700795u },
        {                    0u,  4976477588244398080u,          59806944370u },
        {                    0u, 11529215046068469760u,         114269775390u },
        {                    0u,                    0u,          30625000000u },
        {                    0u,                    0u,          64000000000u },
        {    15035424419724983u,            425795984u,                    0u },
        { 18280822466032836292u,   425795984000815071u,                    0u },
        { 11524250747302615283u, 10043594327130472635u,             23082446u },
        { 15212285943691810760u,  8336034337032909060u,         206544464339u },
        {  7951804027551297019u, 16717215784895280857u,         211451897326u },
        {  1706416229965221847u, 10968831263951212032u,         238906242083u },
        {  3406023111930700826u,  5536629379734406065u,          35594621534u },
        { 16132696204133391302u,  1618806894932332402u,          94300141280u },
        { 11593846688794356915u, 11363331325254998861u,         224087755697u },
        {  6875759884161133906u,  8775167772751754888u,         177616007425u },
        {  3593593325323835965u,  2898202945316114122u,           1475702798u },
        {  9542049733257388925u,  8868842714495185148u,          14157111896u },
        {  9926551925937787518u, 17052094667531999136u,          88480780926u },
        {  3128491553219547895u,  3658615537031138594u,         126924395904u },
        { 17158408656931354885u, 12486952437987190746u,         128198333945u },
        {  2065169543154992616u,   912079238520577629u,         249676919048u },
        {  4832622393556232739u, 10960072898031888041u,           8049443914u },
        { 16505930714733656162u,  6129550094334741912u,          74594146742u },
        { 18270988073492888699u,  7965724516573729480u,         182332283576u },
        {  2525111411519708523u,  5801761178810791992u,         184431822791u },
        { 16728989342518570442u, 13197466483098446115u,         199314514103u },
        {  7974052022039438336u, 11326268638393107273u,         183715436091u },
        {  5356554962386550784u,  3597339351794947378u,          59613998253u },
        {  6693869495028547584u,   353880726151383714u,         173195012157u },
        {  8157517147199766528u, 11154818162602073600u,          61019183912u },
        { 12065776720423157760u,  5141043976157511026u,          40604703904u },
        { 11997589407315001344u,  7188225141808859034u,         160278696552u },
        {                    0u, 13894168943295705185u,         104389674465u },
        {                    0u, 12176538069834828288u,         225753204407u },
        {                    0u,  7994239409235165184u,         183660091451u },
        {                    0u, 13707777025480065024u,          59433368586u },
        {                    0u, 10120227247676719104u,          10743100081u },
        {                    0u,  7358494763030413312u,         177548618618u },
        {                    0u,  7656119366529843200u,         122398904800u },
        {                    0u,  9223372036854775808u,         224415039062u },
        {                    0u,                    0u,          86500000000u },
        {  2418433229320326037u,                 6497u,                    0u },
        {  1143911773589293534u,        6497131103528u,                    0u },
        {  9276183703610924928u,  3877189582299842749u,                  352u },
        {  3532180128827684715u,  7625565791857948238u,          96210182868u },
        {  8563997501322031543u, 16568435163612007484u,         212413382749u },
        { 14843890409658460681u, 17592071940521808130u,          93898176669u },
        {  9489417861634552678u, 15158637878035490831u,         157953668130u },
        {  1113198223322322089u, 17789243229146401893u,          34821751405u },
        { 11190777557146597869u, 14677686051252896484u,         109964356807u },
        { 17764553645932638286u,  3531237481269211986u,         199795678955u },
        { 14900777150991234852u,  8074435404989280910u,         235191428767u },
        { 17364899863357893610u,  7086549341467684427u,         159437716020u },
        { 15537570181590167037u, 10556134770918626963u,          52384162609u },
        {  5558052627121307766u, 10772666134712966775u,          49572249212u },
        { 12095241565795232609u,  6195173298198112620u,         124583987401u },
        {  2187936505958366389u,  8144773843324250887u,         201335841017u },
        { 12009203621325860228u, 14144284817150924691u,         249441529074u },
        { 14814842834750302952u,  6464447844648863176u,         242766763216u },
        {  5965296499605198833u, 15760468443293179135u,         208350438419u },
        {  4051026394962148842u,  5172191224908322475u,          19854376706u },
        { 16799526299141688349u,  2357554307308969012u,           2280385048u },
        {  9460214166646215205u,  1602046917604361745u,          24127803275u },
        { 10750736995029068008u,  7830970218109515845u,         139086847137u },
        {  5377963045376430080u,  2899479134887821084u,         161424517746u },
        { 15996910350253424640u, 15792042302392017912u,         114157181078u },
        { 13651157529655246848u, 11286099112296056199u,         150856088328u },
        {  9771305410219737088u, 15161477829153947404u,           8611820658u },
        { 12709439623416250368u,   423831848142641767u,         114821905360u },
        {  9943947977234055168u,  9707413321046312582u,         208022975970u },
        {                    0u, 10969483299803835620u,         226526239930u },
        {                    0u,  4326479556120930304u,         186594656881u },
        {                    0u, 12876227232041795584u,         113234538926u },
        {                    0u, 16967986827791171584u,         174698021676u },
        {                    0u,  1288146316538413056u,          44919836409u },
        {                    0u, 13715290452691779584u,         249069830551u },
        {                    0u,  4683743612465315840u,         151743507385u },
        {                    0u,                    0u,         185253906250u },
        {                    0u,                    0u,          74000000000u },
        {   371564423966525229u,             99138353u,                    0u },
        { 14464859121514339583u,    99138353020142547u,                    0u },
        {  3913119023023056247u, 16344805304534272784u,              5374300u },
        {  5493396321716566945u,    26429987091348271u,          92886053671u },
        {  5837454566818211973u,  8691371289609838059u,          39001432772u },
        {  2886670683193253881u, 12980168378493046550u,         196471160181u },
        {  5504823105587173817u, 14010125458129496139u,         117703656337u },
        {  9431834277334851106u, 17061829677031795106u,         145759490422u },
        { 12921661346456247087u,  2227928323072698520u,         118924923640u },
        {  3643076516404724246u,  7394752319272287289u,         248120776236u },
        { 12559396953196866477u,  8805771303577744757u,          44400870326u },
        {  1626880974916825698u, 16371027194302248385u,         182477361818u },
        {  6806994170946429566u,  9114324123731231602u,         154887475162u },
        {  7596631230206896579u, 14468189808746991893u,         218494088500u },
        {  3100480253729502401u,  2376054557800684348u,          52784322141u },
        {  6310570748781063286u, 12462238943546048571u,          93128806175u },
        { 14251867077375744136u, 15334855370842605909u,          31675579326u },
        {  6701165793751570137u,  7211347914013798462u,         190831304175u },
        { 10856833140463959923u, 13763642332572548877u,         239390927953u },
        { 15867563727561248556u, 16868268377740071383u,          81746128545u },
        {  5048961008671491600u,  1120013377627684177u,         161914430661u },
        { 13278183119599849051u, 15898107650717274388u,         197060716046u },
        {  4547591784941053655u, 12281923376333274277u,          14861838142u },
        { 11815437715887182496u,  6383530489286615044u,          62665804400u },
        {   398495392178782208u,  4253822060257126466u,         112346051881u },
        {  9154841240825495552u, 17614372438391501998u,          41230600155u },
        {  1902683298245640192u,  4309951310554333450u,         219954877043u },
        {  5081900962138816512u, 13106185988973773020u,         115233642928u },
        {  3234710432358858752u,  2070134359761960547u,         176710487766u },
        { 16717361816799281152u,  9399359914137865875u,         214112222208u },
        {                    0u, 17415053284723541264u,            509540321u },
        {                    0u,  4840502610448261120u,         225944071930u },
        {                    0u,  5690599259712258048u,         250262404172u },
        {                    0u,   114769594245185536u,          76308488004u },
        {                    0u,  3150620882578178048u,          68006221672u },
        {                    0u,  5136918324969472000u,         104170795500u },
        {                    0u,  7205759403792793600u,         236278472900u },
        {                    0u,                    0u,         196390625000u },
        {                    0u,                    0u,         232000000000u },
        { 13488568028574514610u,                 1512u,                    0u },
        {   275784718433886190u,        1512731216738u,                    0u },
        { 10020568880357102364u,    98202693831717807u,                   82u },
        {  8876397213146246746u, 12909287260170414079u,          82005323578u },
        {  2155905919114811310u, 11728631949380786233u,          58699813864u },
        {  1087100407155601220u, 18263701925522197718u,         232635810411u },
        {  2903498381705011170u,  4868886449713321591u,         107990077265u },
        { 12223476257006657765u,  5870139507184082354u,          81263942863u },
        { 12148657163736735595u,  5978562500822661575u,         207318220900u },
        {  8951241323311673823u, 10821136839630268472u,         100324098522u },
        {  8463862715901576300u,  9490907630136752916u,         218586615003u },
        {  3873401978748963266u, 10564005678001613989u,         219514503133u },
        {   248364795947002730u,  5754050547468481222u,         221572675895u },
        { 10454378025404001822u,  3833909949855542602u,          55311927705u },
        {  6574176865628265640u, 15446538552665967784u,         153207836674u },
        {    16703315293848336u, 14924837848804399130u,           2837358532u },
        {  9896033222450013456u, 18140170340418344208u,         196809077080u },
        { 17311376269334085007u, 11380424819825208971u,          88983380604u },
        { 18378511316495639306u, 12416915664152252547u,         124616934065u },
        { 15120796393727584297u, 17195282241626289958u,         177673122346u },
        { 18253447805740347049u,  2649541045825281326u,          42932158118u },
        {  5842497225601731158u, 16577429864268509676u,         166143631907u },
        {  4369968404176723173u, 12051257060168107241u,          35898664273u },
        { 16142207253674488117u,  5363884561143470797u,          81653299954u },
        { 12124259227391928178u, 13054029903083620184u,         242290776764u },
        { 13019483264566077056u,   566314952158634945u,         188707660379u },
        {    74901376448135168u,  1329472079642345682u,          91030699995u },
        { 13897060093813325824u, 15686237486658857211u,         219072070825u },
        {  4495486210810052608u,  1069073549290598129u,         169850352638u },
        { 14885496280087265280u,  4323599065125928507u,         254057954593u },
        {  4976477588244398080u, 17861823329752681586u,          33234382774u },
        { 11529215046068469760u, 17220149985412802078u,         182968291382u },
        {                    0u,  4344934572159429184u,          54933506201u },
        {                    0u,  2252927464837120000u,         153235539375u },
        {                    0u, 10910018171964489728u,         175122131442u },
        {                    0u,  3597328585515335680u,         242591433270u },
        {                    0u,  6972808074239148032u,          54195011573u },
        {                    0u,  2227030015734710272u,         245377996683u },
        {                    0u,  1152921504606846976u,         139120727539u },
        {                    0u,                    0u,         243062500000u },
        {                    0u,                    0u,         160000000000u },
        { 10043594327130472635u,             23082446u,                    0u },
        {  8336034337032909060u,    23082446544464339u,                    0u },
        { 16717215784895280857u, 17238287503805244910u,              1251301u },
        { 10968831263951212032u,  1434575446038410275u,         229934489438u },
        {  5536629379734406065u, 14009569747841241694u,          94077768490u },
        {  1618806894932332402u, 14938795732275951328u,          42759460297u },
        { 11363331325254998861u,  6687653542888983473u,         201809833739u },
        {  8775167772751754888u,    28238723295162625u,          11362538425u },
        {  2898202945316114122u,  4745270274832691214u,         185001530824u },
        {  8868842714495185148u,   926478968112308824u,         200257241617u },
        { 17052094667531999136u,  9213681606604198526u,          17050224525u },
        {  3658615537031138594u, 13346223820579313024u,         141499474680u },
        { 12486952437987190746u,   691642518601291257u,         248723500243u },
        {   912079238520577629u,  1153720150033789192u,         211037494016u },
        { 10960072898031888041u, 12089015034721780810u,             62543294u },
        {  6129550094334741912u,  3555868702841788854u,         190655346818u },
        {  7965724516573729480u, 11708406782758214328u,         130192764028u },
        {  5801761178810791992u,  9417497762905343943u,         124634714003u },
        { 13197466483098446115u, 12838336066957615287u,         147510523576u },
        { 11326268638393107273u, 13737708142128207419u,         184695967592u },
        {  3597339351794947378u, 11683434809834695853u,         104744722650u },
        {   353880726151383714u,  2689114340106315837u,         218633360270u },
        { 11154818162602073600u,  8859225263374261032u,         142145777180u },
        {  5141043976157511026u, 15761671984578600096u,          28480259563u },
        {  7188225141808859034u,  7087267079878005352u,         235854441950u },
        { 13894168943295705185u,  4601291730423121377u,         222384201518u },
        { 12176538069834828288u,  9559411037059581623u,          46249436524u },
        {  7994239409235165184u, 12969820289641388091u,         108518216710u },
        { 13707777025480065024u, 13628239920285957130u,           6703095366u },
        { 10120227247676719104u,  8049893933765800625u,          70738788366u },
        {  7358494763030413312u, 10391755948840250234u,          14436385624u },
        {  7656119366529843200u, 14454650777462444512u,          88563338218u },
        {  9223372036854775808u, 14244638523341127254u,         234783588188u },
        {                    0u, 12246016810439753984u,          92772203401u },
        {                    0u,  9382741764551081984u,         137663857901u },
        {                    0u,  4608696190291148800u,         237508639450u },
        {                    0u,  1696483666416369664u,         218249837921u },
        {                    0u, 15416683541605384192u,          97091966563u },
        {                    0u,  7683140964294066176u,          99835740089u },
        {                    0u,  4611686018427387904u,         185416503906u },
        {                    0u,                    0u,          98250000000u },
        {                    0u,                    0u,         128000000000u },
        {  3877189582299842749u,                  352u,                    0u },
        {  7625565791857948238u,         352210182868u,                    0u },
        { 16568435163612007484u,  1722045467931902045u,                   19u },
        { 17592071940521808130u, 16095324008152856733u,          19093352271u },
        { 15158637878035490831u, 15216188060094280738u,          79872529262u },
        { 17789243229146401893u, 10793385929903030893u,         110824871207u },
        { 14677686051252896484u, 12613277226875940039u,          39585110623u },
        {  3531237481269211986u, 10644539625155600107u,          95683767128u },
        {  8074435404989280910u,  6181262895644173983u,          88577041649u },
        {  7086549341467684427u,   148914399627082292u,         241335086933u },
        { 10556134770918626963u, 14379289774887985969u,          85008072665u },
        { 10772666134712966775u, 11743339675582627452u,         217779502860u },
        {  6195173298198112620u,  7841621929809463497u,          12636607719u },
        {  8144773843324250887u, 11168944680251236601u,         231425095176u },
        { 14144284817150924691u,  6178560202529287410u,           8605469704u },
        {  6464447844648863176u, 13295243308201596112u,           8334940419u },
        { 15760468443293179135u, 17040673746172470291u,           3720736583u },
        {  5172191224908322475u, 14957442487039409922u,          71923776774u },
        {  2357554307308969012u, 17778155426506992152u,           6810844581u },
        {  1602046917604361745u, 14945404984219733899u,         165963755736u },
        {  7830970218109515845u, 11590754866058681505u,         216810192027u },
        {  2899479134887821084u,  6020790784469412466u,         155628336080u },
        { 15792042302392017912u,  7934351824569522326u,         208326387722u },
        { 11286099112296056199u,  5038361112172116744u,          10430122074u },
        { 15161477829153947404u,  3305187319649924210u,          90273130103u },
        {   423831848142641767u, 11470175511099161552u,         119179174563u },
        {  9707413321046312582u,  7308362160352048610u,         163621799460u },
        { 10969483299803835620u, 10666410671225576634u,          36396187106u },
        {  4326479556120930304u,  2181639019945820785u,         226578227281u },
        { 12876227232041795584u,  4615749499734847918u,          81118266888u },
        { 16967986827791171584u, 14076159200958497580u,           8250220281u },
        {  1288146316538413056u,  5470405257862074105u,         249763070119u },
        { 13715290452691779584u,  4565741478181339543u,         167296551263u },
        {  4683743612465315840u,  8901832997861862329u,          95247509341u },
        {                    0u, 14190141170191714122u,          93482569333u },
        {                    0u,  4240772322245764096u,         117769249094u },
        {                    0u,  4422842195340951552u,          70229892728u },
        {                    0u, 15448426386733137920u,         120239762755u },
        {                    0u,  9203504548935630848u,          67837460872u },
        {                    0u,  5936377627571912704u,         136498922981u },
        {                    0u,   468374361246531584u,         229321811676u },
        {                    0u,                    0u,         220025390625u },
        {                    0u,                    0u,          33000000000u },
        { 16344805304534272784u,              5374300u,                    0u },
        {    26429987091348271u,     5374300886053671u,                    0u },
        {  8691371289609838059u,  8020875056524075716u,               291341u },
        { 12980168378493046550u,  1400288714762747253u,          13434812508u },
        { 14010125458129496139u,  6136037711314764689u,          92075909803u },
        { 17061829677031795106u, 15735488086392394102u,         171332635270u },
        {  2227928323072698520u,  7735094782793634552u,         134853022518u },
        {  7394752319272287289u,  7273689191766726188u,          54419320328u },
        {  8805771303577744757u,  3410634565056431030u,           8394307481u },
        { 16371027194302248385u,  4600927904885215898u,         153184890870u },
        {  9114324123731231602u,  9154871331680374746u,         246249416801u },
        { 14468189808746991893u,  6117978272461042996u,          97496286569u },
        {  2376054557800684348u, 13116904339287496285u,         105331656266u },
        { 12462238943546048571u,   867037205615660831u,          74711068809u },
        { 15334855370842605909u,  1802487145191504830u,         137047002181u },
        {  7211347914013798462u, 17242009718457409007u,          69097713023u },
        { 13763642332572548877u, 13620802355488468049u,         127934691219u },
        { 16868268377740071383u,  4442227880594435745u,         147738385175u },
        {  1120013377627684177u, 17354849212854314181u,          23240813655u },
        { 15898107650717274388u, 18202319179831567886u,          87940808260u },
        { 12281923376333274277u, 17568634016348874558u,          68986749699u },
        {  6383530489286615044u,  7496925598312450672u,           3952397558u },
        {  4253822060257126466u,   601870379496813865u,         246406409151u },
        { 17614372438391501998u, 11995106565680728027u,         191032627458u },
        {  4309951310554333450u, 16331071694764184179u,           2650256029u },
        { 13106185988973773020u,  9665962217000524208u,         157885309170u },
        {  2070134359761960547u, 13682661374415474390u,         242523992861u },
        {  9399359914137865875u,  6940361789924260864u,          29741738559u },
        { 17415053284723541264u,  9658039831644010465u,          63376237766u },
        {  4840502610448261120u,  6843715893910236922u,         198523563388u },
        {  5690599259712258048u,    47089792870595660u,         124370998582u },
        {   114769594245185536u, 14510386192097156932u,          54002552742u },
        {  3150620882578178048u, 12059931208360040296u,         166786609611u },
        {  5136918324969472000u, 14877013468459184620u,         203653770180u },
        {  7205759403792793600u,  2397668560671695044u,         196806484516u },
        {                    0u,  2195572305559232232u,          36129977873u },
        {                    0u,  3261686279425953792u,          17119022213u },
        {                    0u,  9333850662059900928u,         133176816367u },
        {                    0u,  5036522340217782272u,         239505989058u },
        {                    0u,  2800120215143186432u,         194273030423u },
        {                    0u,   441634238459019264u,          23151794821u },
        {                    0u,   720575940379279360u,         133023941040u },
        {                    0u,                    0u,         176039062500u },
        {                    0u,                    0u,         228000000000u },
        {    98202693831717807u,                   82u,                    0u },
        { 12909287260170414079u,          82005323578u,                    0u },
        { 11728631949380786233u,  8218347283861607400u,                    4u },
        { 18263701925522197718u, 17896200385973633643u,           4445517498u },
        {  4868886449713321591u, 16333242102094352209u,         186970154966u },
        {  5870139507184082354u,  9981905728606788815u,         214885426828u },
        {  5978562500822661575u, 15219470018924839012u,         140541120193u },
        { 10821136839630268472u, 17152070168529617370u,         193825049122u },
        {  9490907630136752916u, 17841343440958328027u,          34929815586u },
        { 10564005678001613989u, 17291078023923990493u,          34967181165u },
        {  5754050547468481222u, 16744804581790759223u,         109937351217u },
        {  3833909949855542602u,  5001622214111594905u,          49907737675u },
        { 15446538552665967784u,  9676746897435398146u,          75271138483u },
        { 14924837848804399130u,  8109025833995118532u,         179524577500u },
        { 18140170340418344208u,  5495826424046694744u,         220439591171u },
        { 11380424819825208971u,  7890288164365705852u,           3297929347u },
        { 12416915664152252547u,  8616438349039895217u,         131427733378u },
        { 17195282241626289958u, 15787154801788760618u,         130467098058u },
        {  2649541045825281326u, 12418659311480782502u,         202855823376u },
        { 16577429864268509676u,  4486988874116669987u,          16673216870u },
        { 12051257060168107241u,  4828971301551875409u,         102243240154u },
        {  5363884561143470797u, 14769106422014442226u,         218261779058u },
        { 13054029903083620184u,  7763933466423188156u,         114800634863u },
        {   566314952158634945u, 10449097116253839963u,         239420883676u },
        {  1329472079642345682u, 12870692502472900571u,         220566446689u },
        { 15686237486658857211u, 11597479481311003817u,          97697721638u },
        {  1069073549290598129u,  8294994869530047486u,          38628700622u },
        {  4323599065125928507u, 16879315829924478241u,         206449672572u },
        { 17861823329752681586u, 11873324837601439670u,         124915029544u },
        { 17220149985412802078u,  3277599055636107318u,          40643654229u },
        {  4344934572159429184u, 15363467897354242201u,          85177679000u },
        {  2252927464837120000u, 10351182204479784367u,         152832855263u },
        { 10910018171964489728u, 12811517584931924466u,         223561138711u },
        {  3597328585515335680u, 16988930699558748726u,          23694513759u },
        {  6972808074239148032u, 11683499918824718325u,          95920971778u },
        {  2227030015734710272u, 13119300691281647499u,           2633363799u },
        {  1152921504606846976u, 10125549106595354099u,          87711198715u },
        {                    0u, 17505352699870800544u,         251548907116u },
        {                    0u,  6756039242241163264u,         108948967071u },
        {                    0u,  3537338758766526464u,         159366245621u },
        {                    0u,  6522626374119718912u,         245191759518u },
        {                    0u,  4733294203482669056u,         158353592284u },
        {                    0u, 16997710893603094528u,         220256592392u },
        {                    0u, 16717361816799281152u,           8921447753u },
        {                    0u,                    0u,          73906250000u },
        {                    0u,                    0u,          16000000000u },
        { 17238287503805244910u,              1251301u,                    0u },
        {  1434575446038410275u,     1251301934489438u,                    0u },
        { 14009569747841241694u,  3943737498063000362u,                67833u },
        { 14938795732275951328u,  2870731037991212489u,         249213790438u },
        {  6687653542888983473u,  7389433400402095883u,         230155622641u },
        {    28238723295162625u,  5675049236146197433u,         241400581987u },
        {  4745270274832691214u,  9953779846262904264u,          99307645035u },
        {   926478968112308824u, 12691978937179636241u,         107539595486u },
        {  9213681606604198526u, 15523327331528198029u,         222688033556u },
        { 13346223820579313024u, 15722603279568118520u,          20841521260u },
        {   691642518601291257u, 11838632364171816147u,         108852324031u },
        {  1153720150033789192u,  7832751832367143680u,         191641773546u },
        { 12089015034721780810u, 12167724027162940862u,         234424614327u },
        {  3555868702841788854u,  4108211144748152962u,         183659613641u },
        { 11708406782758214328u,  7530983398136343676u,         201222706572u },
        {  9417497762905343943u,  1117587133956542355u,         140408255428u },
        { 12838336066957615287u, 17134748625149490872u,         196060584519u },
        { 13737708142128207419u,  4039918359454207848u,          71928876584u },
        { 11683434809834695853u,  1830218764589441242u,          40219004413u },
        {  2689114340106315837u,   637895981480825742u,         253099216358u },
        {  8859225263374261032u,  8246879226348334620u,         230034580410u },
        { 15761671984578600096u, 12389239568142583275u,         186447064218u },
        {  7087267079878005352u, 14041257178803154398u,         154671622022u },
        {  4601291730423121377u, 16312515716494630702u,         134761178076u },
        {  9559411037059581623u, 17088522799596987756u,         220884303248u },
        { 12969820289641388091u,  3588932524637852678u,         144926370677u },
        { 13628239920285957130u,   107218049069817414u,         117194556422u },
        {  8049893933765800625u,  1596707240462008334u,           6005812302u },
        { 10391755948840250234u, 17461913142391587672u,          78086557672u },
        { 14454650777462444512u,  4366474266651610090u,         232946612208u },
        { 14244638523341127254u,  5539304013194805084u,         240236707044u },
        { 12246016810439753984u,  4762470619211987849u,         228300286272u },
        {  9382741764551081984u, 10835638458986644717u,          64258174049u },
        {  4608696190291148800u, 16141642290510052058u,          97587401137u },
        {  1696483666416369664u, 17390568670756355425u,         177875040181u },
        { 15416683541605384192u, 12536768491333867107u,         181942744616u },
        {  7683140964294066176u, 13145148522871947193u,          40679619581u },
        {  4611686018427387904u,  5665349945233068642u,         253712599929u },
        {                    0u, 17074607537751066240u,         121307119235u },
        {                    0u,  6241525660962062336u,         131925616329u },
        {                    0u,  1142860629783085056u,         201338353784u },
        {                    0u, 16287527416870469632u,         120061954598u },
        {                    0u,  9028002014738513920u,          38882948630u },
        {                    0u, 16217462258161156096u,          22489408969u },
        {                    0u, 11529215046068469760u,         201879150390u },
        {                    0u,                    0u,          54625000000u },
        {                    0u,                    0u,          64000000000u },
        {  1722045467931902045u,                   19u,                    0u },
        { 16095324008152856733u,          19093352271u,                    0u },
        { 15216188060094280738u,   646608198162977646u,                    1u },
        { 10793385929903030893u, 12170458846894708007u,           1035052700u },
        { 12613277226875940039u,  1797330480103086687u,         156659761896u },
        { 10644539625155600107u, 10332188564497263448u,         232097433480u },
        {  6181262895644173983u,  7524259485079594225u,         136560109064u },
        {   148914399627082292u,    62681109059153749u,           8407890924u },
        { 14379289774887985969u, 13480636451804037081u,         236003397949u },
        { 11743339675582627452u,  6948168233012789004u,          61730786766u },
        {  7841621929809463497u, 12015502974041806055u,         206376660954u },
        { 11168944680251236601u,  7343801660689004040u,         218651361721u },
        {  6178560202529287410u, 13670580858640731144u,         185398108285u },
        { 13295243308201596112u,  5605073897566574851u,         125741083673u },
        { 17040673746172470291u, 15387788940505247559u,          25303851664u },
        { 14957442487039409922u, 17565181499678113030u,         144834173709u },
        { 17778155426506992152u,  1893743623847493029u,          13952210397u },
        { 14945404984219733899u, 10243498996716269784u,         221102660047u },
        { 11590754866058681505u,  5619675836950314139u,         207555301193u },
        {  6020790784469412466u, 10224869737511515088u,          73304643237u },
        {  7934351824569522326u,  2574495974386198538u,         165554291299u },
        {  5038361112172116744u,  7825756347302873178u,          99139563706u },
        {  3305187319649924210u, 12071550103794656887u,         186424235101u },
        { 11470175511099161552u,  7195875213867606691u,          93654400042u },
        {  7308362160352048610u, 18271364438406891044u,          42390089176u },
        { 10666410671225576634u, 16966521933952564706u,         216990492650u },
        {  2181639019945820785u,   289920862029570129u,         234919756997u },
        {  4615749499734847918u,  7804199568098625032u,         197015716641u },
        { 14076159200958497580u,  5758118571242446585u,          33423066506u },
        {  5470405257862074105u,  4030788293606375591u,         138312148233u },
        {  4565741478181339543u,  4387716460037196127u,           9218509471u },
        {  8901832997861862329u, 16807506478881285981u,         159237858585u },
        { 14190141170191714122u, 17033060604413529717u,          25911136751u },
        {  4240772322245764096u, 10498418508292170054u,         239923364065u },
        {  4422842195340951552u, 13237752038744465016u,         225569120407u },
        { 15448426386733137920u, 17737618428304633155u,         151717619975u },
        {  9203504548935630848u, 13546183833248825736u,           7961558221u },
        {  5936377627571912704u,   826778452978976229u,         205734340097u },
        {   468374361246531584u, 13728076626990147292u,           1044819749u },
        {                    0u,  2794860281883592225u,          37744200525u },
        {                    0u,  8680705720425908736u,          77151509679u },
        {                    0u,   731520517439488000u,         175470582000u },
        {                    0u, 13120812320768917504u,         240039655806u },
        {                    0u,  2722954908557901824u,         126711280661u },
        {                    0u,  6860847004205973504u,          21147611681u },
        {                    0u,  6503197861922996224u,          33371927261u },
        {                    0u,  9223372036854775808u,         221352539062u },
        {                    0u,                    0u,         182500000000u },
        {  8020875056524075716u,               291341u,                    0u },
        {  1400288714762747253u,      291341434812508u,                    0u },
        {  6136037711314764689u, 12005656413127238315u,                15793u },
        { 15735488086392394102u,  4821130826186787462u,         177650827938u },
        {  7735094782793634552u, 14377899467066168118u,         162261354025u },
        {  7273689191766726188u, 16575613239625444872u,          41779427491u },
        {  3410634565056431030u,  4317827099179284377u,         163898565794u },
        {  4600927904885215898u,  1242354770412171254u,         162234069876u },
        {  9154871331680374746u,   994838588328896609u,         116067348187u },
        {  6117978272461042996u, 17283309862013060457u,         219053930307u },
        { 13116904339287496285u,   124242522249856586u,          67936930105u },
        {   867037205615660831u, 11564608014666985609u,          57006735200u },
        {  1802487145191504830u, 12401028575581654085u,          96626918656u },
        { 17242009718457409007u,  2490725392961465727u,            672261106u },
        { 13620802355488468049u,  1949482237120640915u,         242135022494u },
        {  4442227880594435745u, 15410502396166200087u,         158105681643u },
        { 17354849212854314181u, 15694919529799920727u,         235835405008u },
        { 18202319179831567886u, 10324869370171768388u,         208850823292u },
        { 17568634016348874558u,  1631866459122189059u,         124559712290u },
        {  7496925598312450672u,   172020494461226230u,          34088463658u },
        {   601870379496813865u, 12734610307908856767u,          42009325249u },
        { 11995106565680728027u,  1467513250829340930u,         193690344608u },
        { 16331071694764184179u, 13558759428494307997u,         160079554052u },
        {  9665962217000524208u,  7915355143999496434u,           4735021821u },
        { 13682661374415474390u,  2876370200608797469u,         253429092262u },
        {  6940361789924260864u,   343685370404989503u,         166155928341u },
        {  9658039831644010465u,  4837266557407634630u,          21018631221u },
        {  6843715893910236922u,  9622591415747161468u,          53262228745u },
        {    47089792870595660u, 16503783814424220982u,           9521641725u },
        { 14510386192097156932u,  5377083431343591334u,         253894671913u },
        { 12059931208360040296u, 16508482371299291595u,          41291492276u },
        { 14877013468459184620u, 10515883558812249028u,         180894926622u },
        {  2397668560671695044u,    63492062913405476u,          30570067190u },
        {  2195572305559232232u, 11571919759617799697u,         246003441911u },
        {  3261686279425953792u,  2956602334970088581u,         247627315027u },
        {  9333850662059900928u, 13604736747717849839u,          83160277733u },
        {  5036522340217782272u, 16573540719338151362u,         229737514256u },
        {  2800120215143186432u, 12620703004601168151u,          16898453442u },
        {   441634238459019264u, 14649407809089591941u,         194684169680u },
        {   720575940379279360u, 11290375247898624432u,         208794145988u },
        {                    0u, 11020319450292874212u,         196612052468u },
        {                    0u,  8754634933362354176u,         244597412714u },
        {                    0u, 12976319450332528640u,         106474589710u },
        {                    0u, 17447331119627239424u,          14703447686u },
        {                    0u,  3665184902673858560u,         134945821715u },
        {                    0u, 12949678516038795264u,          19198690071u },
        {                    0u,    72057594037927936u,          23702003479u },
        {                    0u,                    0u,          23003906250u },
        {                    0u,                    0u,         202000000000u },
        {  8218347283861607400u,                    4u,                    0u },
        { 17896200385973633643u,           4445517498u,                    0u },
        { 16333242102094352209u,  4445517498970154966u,                    0u },
        {  9981905728606788815u,  9413159735776077452u,            240991986u },
        { 15219470018924839012u, 14279163482889998017u,         242510288411u },
        { 17152070168529617370u,  8693044629541194274u,          27774075003u },
        { 17841343440958328027u, 11863110253260222498u,         123471250893u },
        { 17291078023923990493u,  8319293368489531245u,         205643100495u },
        { 16744804581790759223u,  3376307525676489265u,          79450989797u },
        {  5001622214111594905u, 13205662254759912523u,         229183029997u },
        {  9676746897435398146u,  5276250334231686323u,         237715880385u },
        {  8109025833995118532u, 13790198520922745052u,         193286026103u },
        {  5495826424046694744u, 14195535250150996227u,         119747568159u },
        {  7890288164365705852u, 16425228796427004035u,          31769541507u },
        {  8616438349039895217u,  4295900841296269186u,         131890413437u },
        { 15787154801788760618u,  4533952595483946442u,         125232881251u },
        { 12418659311480782502u, 12885038019373447184u,          99245786062u },
        {  4486988874116669987u, 12140736240487831910u,         206698499310u },
        {  4828971301551875409u,  6927124077155322074u,         238658150630u },
        { 14769106422014442226u, 12477788342407819890u,         230375520148u },
        {  7763933466423188156u,  7980854329409711087u,         148676422261u },
        { 10449097116253839963u,  2062671021810827996u,         117432642980u },
        { 12870692502472900571u,  2739521363598172769u,         164111817620u },
        { 11597479481311003817u, 12897585686593465638u,         148148509750u },
        {  8294994869530047486u,  1127632646629044686u,          54699179521u },
        { 16879315829924478241u,  4833775019274666364u,           1061129088u },
        { 11873324837601439670u, 15867662672939849256u,         128262039468u },
        {  3277599055636107318u,  2092350330982953557u,         172860187717u },
        { 15363467897354242201u, 13330062299842493592u,          69113426538u },
        { 10351182204479784367u,  4479193352178519263u,         106722624125u },
        { 12811517584931924466u,  3149393938889064983u,         125242817558u },
        { 16988930699558748726u,  9736379904070620767u,          22170728987u },
        { 11683499918824718325u,  3816238703055069186u,          27527810212u },
        { 13119300691281647499u, 11598915938798661975u,         164206878714u },
        { 10125549106595354099u, 17821633264606555643u,         250628778492u },
        { 17505352699870800544u,  2514623558764574316u,         252966112675u },
        {  6756039242241163264u,  4976730480406253215u,         163136318016u },
        {  3537338758766526464u, 17276563697191611637u,          64269789099u },
        {  6522626374119718912u, 12524734095940998814u,         171936564394u },
        {  4733294203482669056u, 15331551308930355164u,         170678967195u },
        { 16997710893603094528u, 15417115581125943816u,         155831125061u },
        { 16717361816799281152u,  6010750237807115593u,          69835763510u },
        {                    0u,  5624630987553628432u,          54325843423u },
        {                    0u, 14881848243837640704u,         223304911856u },
        {                    0u, 15281613886881529856u,         240806746609u },
        {                    0u, 14057902358273196032u,         241828417948u },
        {                    0u, 16075318494433902592u,         156762080413u },
        {                    0u, 13891916000577716224u,         157871444761u },
        {                    0u,  7205759403792793600u,          25753082275u },
        {                    0u,                    0u,         163390625000u },
        {                    0u,                    0u,         232000000000u },
        {  3943737498063000362u,                67833u,                    0u },
        {  2870731037991212489u,       67833213790438u,                    0u },
        {  7389433400402095883u,  4535831408134330609u,                 3677u },
        {  5675049236146197433u,  6204770794376564579u,          93245887913u },
        {  9953779846262904264u, 13869812122751887467u,         169336361298u },
        { 12691978937179636241u, 14253229412394467550u,          82751884021u },
        { 15523327331528198029u, 12776557610216045332u,         245772669114u },
        { 15722603279568118520u, 16493640728678654060u,         186692618575u },
        { 11838632364171816147u,  9434398296825833151u,          79894122055u },
        {  7832751832367143680u,  8773374058285327850u,          71511439756u },
        { 12167724027162940862u, 12932015276748029367u,         140475605560u },
        {  4108211144748152962u, 16293958583527755209u,          56701045952u },
        {  7530983398136343676u, 13511893936143127948u,         192883297264u },
        {  1117587133956542355u, 18409936402005226436u,         240732481237u },
        { 17134748625149490872u,  2189663026458466887u,         213998004652u },
        {  4039918359454207848u,  9497725274248154664u,         172118701870u },
        {  1830218764589441242u, 14766925481127792125u,          46514872718u },
        {   637895981480825742u,  6982373971809635814u,         142800516634u },
        {  8246879226348334620u,  8616702383006884794u,          26378515251u },
        { 12389239568142583275u,  3059473300040871066u,          51467112372u },
        { 14041257178803154398u, 17123843157031495558u,         180165854379u },
        { 16312515716494630702u, 11210627174210626524u,         171928285397u },
        { 17088522799596987756u, 15868067138625928592u,         213607729316u },
        {  3588932524637852678u,  4467869511636937589u,         164860209643u },
        {   107218049069817414u, 10052108125844341766u,         235242203691u },
        {  1596707240462008334u,  7470588003218451534u,          43544925873u },
        { 17461913142391587672u,  2613527085490786280u,         177404981387u },
        {  4366474266651610090u,  3632919450036549616u,         139141679587u },
        {  5539304013194805084u,   179367907231218916u,         227196940958u },
        {  4762470619211987849u, 13553068184555874624u,         158009723553u },
        { 10835638458986644717u,  8798774862365584481u,         161734713298u },
        { 16141642290510052058u,   910911255817064881u,         210476982541u },
        { 17390568670756355425u,  2304331144765093813u,          13049380598u },
        { 12536768491333867107u, 12248937023083640360u,         246124918041u },
        { 13145148522871947193u, 10206039550662130685u,          25664016206u },
        {  5665349945233068642u, 12267881323837852537u,          78553270512u },
        { 17074607537751066240u,  2858642007937891971u,         240665043179u },
        {  6241525660962062336u, 14171330289750320841u,         235154967293u },
        {  1142860629783085056u,  6601103619749017720u,         253768229354u },
        { 16287527416870469632u,  4919573414486739494u,         234357846544u },
        {  9028002014738513920u,  3401998285294974486u,          16266690609u },
        { 16217462258161156096u, 10799436256515532233u,          49184422696u },
        { 11529215046068469760u, 10083786644665753398u,          40585438612u },
        {                    0u,  6481194517685688896u,         148546643169u },
        {                    0u, 15104161756860547072u,         225351346258u },
        {                    0u,  9556039274244079616u,          82818798249u },
        {                    0u,  1376343134954323968u,         169518033927u },
        {                    0u, 15682488278596976640u,           7074611710u },
        {                    0u,  1506454075355430912u,         254850149393u },
        {                    0u,  1152921504606846976u,          17081665039u },
        {                    0u,                    0u,          15062500000u },
        {                    0u,                    0u,         160000000000u },
        { 12170458846894708007u,           1035052700u,                    0u },
        {  1797330480103086687u,  1035052700659761896u,                    0u },
        { 10332188564497263448u,  6172559441576707976u,             56110319u },
        {  7524259485079594225u, 15083329738554729992u,         239334615117u },
        {    62681109059153749u, 10013126833549229036u,          77817668943u },
        { 13480636451804037081u,  5817156823499936061u,          79542812693u },
        {  6948168233012789004u,  5282692560913632718u,          21315348703u },
        { 12015502974041806055u, 10252307034225766362u,         223286375337u },
        {  7343801660689004040u, 17981881283247669689u,         169555778677u },
        { 13670580858640731144u, 11689290159733383293u,         117974799737u },
        {  5605073897566574851u,  5530668968487988249u,         121633677689u },
        { 15387788940505247559u, 10083765740821947024u,         121299818165u },
        { 17565181499678113030u,  2798423656816843533u,         181546642036u },
        {  1893743623847493029u,  7614494481582904797u,         116151702850u },
        { 10243498996716269784u, 17811318500083423695u,          66412782572u },
        {  5619675836950314139u, 11641467412200329033u,         236965553510u },
        { 10224869737511515088u, 17733593025296340645u,         102631085212u },
        {  2574495974386198538u,  3689424000190644835u,         156961340004u },
        {  7825756347302873178u, 14966634145516728506u,         100200004075u },
        { 12071550103794656887u, 14171681941562070109u,         235811342862u },
        {  7195875213867606691u,  8130575762882608170u,          14768248417u },
        { 18271364438406891044u,  5234550794400656856u,          97440759395u },
        { 16966521933952564706u,  3020576149360486378u,          99283765567u },
        {   289920862029570129u,  3038675756589057221u,          63163745761u },
        {  7804199568098625032u, 15470260187120878369u,         225164726942u },
        {  5758118571242446585u,  3497929414841828746u,         158838644485u },
        {  4030788293606375591u,  9935840636861015305u,           5189623133u },
        {  4387716460037196127u,  3647355485153741471u,          93538623000u },
        { 16807506478881285981u,   766100215038272793u,          24197723537u },
        { 17033060604413529717u, 16128087474216800751u,         145041530375u },
        { 10498418508292170054u, 16216631732633731297u,           7874305373u },
        { 13237752038744465016u, 13760220872779997335u,          93879105367u },
        { 17737618428304633155u,  3826276262374222087u,          87745943068u },
        { 13546183833248825736u, 14938032745839181005u,          28207422851u },
        {   826778452978976229u, 14479259995009508865u,         131809792377u },
        { 13728076626990147292u,  2372033248156102437u,         121784922257u },
        {  2794860281883592225u,   792005346826701645u,         145128588180u },
        {  8680705720425908736u, 16278924527931792559u,         148042934695u },
        {   731520517439488000u, 17442516423538940144u,         167882482266u },
        { 13120812320768917504u,    13844184233048446u,          90945560710u },
        {  2722954908557901824u, 13486193870480782357u,         134000750494u },
        {  6860847004205973504u, 11931315179184648737u,         158731088034u },
        {  6503197861922996224u, 16492562205587485405u,         162646797891u },
        {  9223372036854775808u, 12128987217680380854u,          67894063588u },
        {                    0u, 10568123814189138176u,         228657513714u },
        {                    0u, 17007583519117541376u,         242572899139u },
        {                    0u,   143791533903052800u,          67921982950u },
        {                    0u, 12398714235792654336u,         230007794954u },
        {                    0u,  9659957317919047680u,          10672135645u },
        {                    0u,  9412523221204336640u,         221523667335u },
        {                    0u,  4611686018427387904u,         135510253906u },
        {                    0u,                    0u,          82250000000u },
        {                    0u,                    0u,         128000000000u },
        { 12005656413127238315u,                15793u,                    0u },
        {  4821130826186787462u,       15793650827938u,                    0u },
        { 14377899467066168118u,  3237900842885170729u,                  856u },
        { 16575613239625444872u,  7515893506498066595u,          88175526956u },
        {  4317827099179284377u,  7300206309181072546u,          44407437403u },
        {  1242354770412171254u,     5999737279837044u,          91395744977u },
        {   994838588328896609u,  7556839307242450651u,         209000325246u },
        { 17283309862013060457u, 12946035041643640643u,         126409657079u },
        {   124242522249856586u, 15885877642352740665u,         247701805965u },
        { 11564608014666985609u, 10770818348246089568u,         141861175152u },
        { 12401028575581654085u, 11635415503599551744u,         112583887232u },
        {  2490725392961465727u,  6248053924100826098u,         128630757138u },
        {  1949482237120640915u, 16894170802729859998u,          18338707681u },
        { 15410502396166200087u,  6143589029651889899u,         225915834834u },
        { 15694919529799920727u, 11812087701837886160u,         210333044628u },
        { 10324869370171768388u,  7306705080150829180u,         148640334557u },
        {  1631866459122189059u,  1485332570280714274u,         221396097276u },
        {   172020494461226230u, 18042602303295630634u,         252080520039u },
        { 12734610307908856767u, 13397029889257074369u,         103978091430u },
        {  1467513250829340930u,  9948104869613411488u,         166726254445u },
        { 13558759428494307997u, 10836066241170646532u,         109539287845u },
        {  7915355143999496434u, 18330574781234459389u,          37587424327u },
        {  2876370200608797469u,   666297360208433062u,          71993702450u },
        {   343685370404989503u,  5035352224889324309u,          50036120052u },
        {  4837266557407634630u,  1341745796439923765u,         244272966991u },
        {  9622591415747161468u,  6846932182653803785u,          79072736185u },
        { 16503783814424220982u,  6727685027257825533u,         185371172937u },
        {  5377083431343591334u,  2168538874806877737u,          73364708536u },
        { 16508482371299291595u, 17694936100676971444u,         184117556727u },
        { 10515883558812249028u,  2163944241059563294u,         247959244408u },
        {    63492062913405476u,  6727780864524301558u,         120117307652u },
        { 11571919759617799697u,  8599551977795002615u,           4364713731u },
        {  2956602334970088581u, 15428264807806859091u,           3466182646u },
        { 13604736747717849839u,  2126771385339683557u,         246836367911u },
        { 16573540719338151362u, 15094316562082972944u,          39115292507u },
        { 12620703004601168151u,  8111300598225956802u,          91818264540u },
        { 14649407809089591941u,  9481215200564260304u,         220439714486u },
        { 11290375247898624432u, 16836674128623424708u,         182513977705u },
        { 11020319450292874212u,  7087243115299722740u,         105912717933u },
        {  8754634933362354176u,  2343560867338408810u,         109384200219u },
        { 12976319450332528640u,  3431385749090422286u,          27127044689u },
        { 17447331119627239424u,  3504545517469224582u,          81186015794u },
        {  3665184902673858560u,  3333759805712094227u,          50189981793u },
        { 12949678516038795264u,  3595183476205994775u,          97180723481u },
        {    72057594037927936u, 14191566632569921303u,          25194895286u },
        {                    0u, 12917427671358095562u,         182769326368u },
        {                    0u,  3883793922738316288u,          32700255157u },
        {                    0u,  7857281689266421760u,         181210540890u },
        {                    0u, 15987081651486195712u,          90425944093u },
        {                    0u, 16827562156399525888u,          29866661432u },
        {                    0u,  7012737938513461248u,          56912223972u },
        {                    0u,  7385903388887613440u,         228380161285u },
        {                    0u,                    0u,           5400390625u },
        {                    0u,                    0u,         225000000000u },
        {  9413159735776077452u,            240991986u,                    0u },
        { 14279163482889998017u,   240991986510288411u,                    0u },
        {  8693044629541194274u, 14135788013842776187u,             13064201u },
        { 11863110253260222498u, 13284322918167594445u,           9766302603u },
        {  8319293368489531245u,  7264587765474046287u,         139720144588u },
        {  3376307525676489265u, 16176482219778368741u,         204393814091u },
        { 13205662254759912523u,  5401983818872095469u,          75876928858u },
        {  5276250334231686323u, 11208857446851049921u,          90292842129u },
        { 13790198520922745052u, 13794690008281035639u,         145607633379u },
        { 14195535250150996227u, 14519782740993303071u,         227747811643u },
        { 16425228796427004035u, 10885858587044789123u,          59787118999u },
        {  4295900841296269186u,  8710500938899914621u,         151590123576u },
        {  4533952595483946442u,  1284182587483102819u,          56472197202u },
        { 12885038019373447184u, 10346074482131502030u,          82069615677u },
        { 12140736240487831910u,  9429804686255246574u,          61560861821u },
        {  6927124077155322074u,  6412022633845121254u,         125511190736u },
        { 12477788342407819890u,  8892351297529018260u,         208347596443u },
        {  7980854329409711087u, 14098160105983060597u,         155482055329u },
        {  2062671021810827996u, 13793833029739474340u,         161764262790u },
        {  2739521363598172769u, 16367653765996977044u,         134747765186u },
        { 12897585686593465638u, 10684788343333772342u,         194887292288u },
        {  1127632646629044686u, 13272681218705145345u,         128579223536u },
        {  4833775019274666364u, 11093568615497829248u,         240719513490u },
        { 15867662672939849256u, 12488220765137758124u,         146601383559u },
        {  2092350330982953557u,  3727114642519696453u,         135676987804u },
        { 13330062299842493592u, 11549865375695057514u,         156202047289u },
        {  4479193352178519263u, 11292809154908783229u,          57626119456u },
        {  3149393938889064983u, 17723904861837310998u,          32612184410u },
        {  9736379904070620767u, 14877674388187150875u,          90960814807u },
        {  3816238703055069186u, 12178961950105734308u,         215806520344u },
        { 11598915938798661975u,  4540604068069253114u,          24660222850u },
        { 17821633264606555643u, 13832478722153359868u,         130246146639u },
        {  2514623558764574316u,  1308046668730371491u,          79749860174u },
        {  4976730480406253215u, 18400531023544756800u,          78070909351u },
        { 17276563697191611637u,  9789823458621466539u,         167997494785u },
        { 12524734095940998814u,  1924870562610267306u,           1530707393u },
        { 15331551308930355164u,  5290016144582400923u,         193104347442u },
        { 15417115581125943816u, 15162883663174059077u,          50286772349u },
        {  6010750237807115593u,  8078086116520046390u,         125821981570u },
        {  5624630987553628432u, 15731407332173190623u,         130437913925u },
        { 14881848243837640704u,  5346389182763011056u,          69852801300u },
        { 15281613886881529856u,  6368422217216252401u,          20289828338u },
        { 14057902358273196032u,  2961453088119116188u,         242345232860u },
        { 16075318494433902592u, 10932141691610170525u,         220160540693u },
        { 13891916000577716224u, 11034016191361782553u,          21592632588u },
        {  7205759403792793600u,  5455325785621453219u,          12598155216u },
        {                    0u,  7735615202566149352u,         208295733803u },
        {                    0u,  7502396497775759360u,          43419348540u },
        {                    0u,  1601286435751591936u,          60406705729u },
        {                    0u, 11449383158571597824u,          65086805911u },
        {                    0u, 13043944595690356736u,         151620672304u },
        {                    0u,  7773494431818186752u,          48707113653u },
        {                    0u,  9943947977234055168u,         181421401977u },
        {                    0u,                    0u,         121539062500u },
        {                    0u,                    0u,         228000000000u },
        {  4535831408134330609u,                 3677u,                    0u },
        {  6204770794376564579u,        3677245887913u,                    0u },
        { 13869812122751887467u,  6343817245135589714u,                  199u },
        { 14253229412394467550u, 17549323075660516085u,         199343899021u },
        { 12776557610216045332u,  3948641822109421754u,         141951350710u },
        { 16493640728678654060u,  1750739713693534543u,         182214056302u },
        {  9434398296825833151u,   962163898128633415u,         110094907790u },
        {  8773374058285327850u,  7967320249386531212u,         142052159009u },
        { 12932015276748029367u,  3018466665533383224u,          33431909296u },
        { 16293958583527755209u, 15076865731854945472u,         176163631405u },
        { 13511893936143127948u,   691187172844604400u,          45817318529u },
        { 18409936402005226436u, 13274492813370992341u,         129037469331u },
        {  2189663026458466887u,  6364168818499152300u,         147719611697u },
        {  9497725274248154664u, 17599380787401914158u,          49345002282u },
        { 14766925481127792125u,  3782323149461692814u,          42954064344u },
        {  6982373971809635814u, 14470163442442237466u,         216205040148u },
        {  8616702383006884794u,   476109872130437939u,          20784429132u },
        {  3059473300040871066u, 16330548844673355700u,          76025809967u },
        { 17123843157031495558u, 14089158961463739563u,          47885280826u },
        { 11210627174210626524u, 13385510793074798805u,          58763774837u },
        { 15868067138625928592u,  1549401308746959012u,         117725629994u },
        {  4467869511636937589u,  4607384943843027435u,          42083993213u },
        { 10052108125844341766u,  5157353797716093483u,         125249766838u },
        {  7470588003218451534u, 10846828782671550129u,         182279580709u },
        {  2613527085490786280u,  9915857350819131531u,          37588007766u },
        {  3632919450036549616u,  1673544973504317923u,          86537539704u },
        {   179367907231218916u, 14780986291622785694u,         120090723054u },
        { 13553068184555874624u,  8168111319515466401u,         238801278872u },
        {  8798774862365584481u, 16345760387859734482u,         152442794201u },
        {   910911255817064881u,  3177475373321281805u,         217886105446u },
        {  2304331144765093813u,  2558676822419554038u,         102172251285u },
        { 12248937023083640360u,  8813474062662382873u,         149138706148u },
        { 10206039550662130685u,  5426294560236228430u,         228477779386u },
        { 12267881323837852537u,  9919177474128333040u,         186294160017u },
        {  2858642007937891971u,  6197383943089627371u,         145537719688u },
        { 14171330289750320841u, 13673239314867423997u,         136335960856u },
        {  6601103619749017720u,  9309584098968723946u,          24741227788u },
        {  4919573414486739494u,  4647101757759615504u,          12504673565u },
        {  3401998285294974486u,  1405809295505096753u,          29251919891u },
        { 10799436256515532233u, 11332704079573859112u,          19076209074u },
        { 10083786644665753398u,  2960072434514044308u,         178614347119u },
        {  6481194517685688896u,  3887266602785432801u,         111160465848u },
        { 15104161756860547072u, 14545546084687849554u,         184210729144u },
        {  9556039274244079616u,  4617763804182385321u,         184788515633u },
        {  1376343134954323968u,  7857823815580249095u,          49250329477u },
        { 15682488278596976640u, 10939326736548364798u,         133425973482u },
        {  1506454075355430912u, 12262012446566951953u,         234593022090u },
        {  1152921504606846976u, 12555024338687723023u,         138664725026u },
        {                    0u,  3332969632922829472u,          34680609233u },
        {                    0u, 15535060143360327680u,         209180680645u },
        {                    0u, 15794322927987458048u,         197842157297u },
        {                    0u, 10571474314433921024u,         241856211961u },
        {                    0u, 16679514427547975680u,         249573080770u },
        {                    0u, 16925653299565166592u,         194904198288u },
        {                    0u, 16717361816799281152u,         144917541503u },
        {                    0u,                    0u,         127906250000u },
        {                    0u,                    0u,          16000000000u },
        {  6172559441576707976u,             56110319u,                    0u },
        { 15083329738554729992u,    56110319334615117u,                    0u },
        { 10013126833549229036u,  9335385384027907407u,              3041746u },
        {  5817156823499936061u, 13237828406194798613u,         210506072255u },
        {  5282692560913632718u, 15667486867836528863u,         191717624115u },
        { 10252307034225766362u, 17982325043592934313u,          51849336164u },
        { 17981881283247669689u, 17159117626917379189u,         100974823793u },
        { 11689290159733383293u,  8336208968408929657u,         113930197630u },
        {  5530668968487988249u, 12767090573379150201u,         126451906793u },
        { 10083765740821947024u, 14736070002412246709u,         233692105366u },
        {  2798423656816843533u,  9697296975344560756u,         150798843955u },
        {  7614494481582904797u,  7291706381199103298u,          51525691522u },
        { 17811318500083423695u, 18098546597780825068u,         130395284194u },
        { 11641467412200329033u,   132913902678533478u,         226981124177u },
        { 17733593025296340645u,  1879347741692007580u,          81007205277u },
        {  3689424000190644835u,  4056624629214083684u,         157101879645u },
        { 14966634145516728506u, 14713227692042795499u,          93219910061u },
        { 14171681941562070109u,  7366415124022528526u,         173797605671u },
        {  8130575762882608170u,   825770353378039393u,          39399334164u },
        {  5234550794400656856u, 10244023944395357795u,          20044765100u },
        {  3020576149360486378u, 14302658294713551167u,         172555329650u },
        {  3038675756589057221u, 14246653166206862817u,         114775348659u },
        { 15470260187120878369u, 12404486258134291102u,         179772312615u },
        {  3497929414841828746u,  8887442218637942533u,          39672448547u },
        {  9935840636861015305u,  1186724038081863005u,          35481789208u },
        {  3647355485153741471u,   211331772484951576u,          24064332439u },
        {   766100215038272793u,  6311919513247413649u,         151011456318u },
        { 16128087474216800751u,  8131780018703965703u,          62342169842u },
        { 16216631732633731297u,  2262544347226725725u,         242440824678u },
        { 13760220872779997335u, 15318188749880522583u,         102122652774u },
        {  3826276262374222087u,  1073117094162650652u,         102830400676u },
        { 14938032745839181005u,  4447950380665871747u,         164058173794u },
        { 14479259995009508865u,  5373227185066463609u,          98241123873u },
        {  2372033248156102437u,  6739731406934274193u,          33291283229u },
        {   792005346826701645u, 12328812617001239444u,          29365361571u },
        { 16278924527931792559u,  3246111484407310759u,         163668346271u },
        { 17442516423538940144u,  3250825415176839770u,         159175972056u },
        {    13844184233048446u, 16146270540000862342u,         216176227598u },
        { 13486193870480782357u, 15686773375425916830u,          14875291079u },
        { 11931315179184648737u, 11920791905793880226u,         199850381688u },
        { 16492562205587485405u,  1853290561644080707u,         120646227424u },
        { 12128987217680380854u, 12157689141506159076u,         224100467082u },
        { 10568123814189138176u, 18100318838862562546u,         138659069648u },
        { 17007583519117541376u,  7171257882533475139u,         208981220250u },
        {   143791533903052800u, 14477550873015039462u,         154388754668u },
        { 12398714235792654336u,  8109481182495403274u,         236784829605u },
        {  9659957317919047680u, 14565395719337663965u,         165439615855u },
        {  9412523221204336640u,  1860318978161305991u,         111789591684u },
        {  4611686018427387904u, 16268646275151585618u,         132100848093u },
        {                    0u, 13759019338835519104u,         221881925081u },
        {                    0u, 17003783176010661888u,         217745877932u },
        {                    0u, 18357489540307877888u,         172921776932u },
        {                    0u,   905481790074912768u,          36995161502u },
        {                    0u,  3638882110636294144u,         158049086266u },
        {                    0u,  9011702854368362496u,          58197264194u },
        {                    0u, 11529215046068469760u,          66488525390u },
        {                    0u,                    0u,          78625000000u },
        {                    0u,                    0u,          64000000000u },
        {  3237900842885170729u,                  856u,                    0u },
        {  7515893506498066595u,         856175526956u,                    0u },
        {  7300206309181072546u,  7625299565768063067u,                   46u },
        {     5999737279837044u, 13889021769065194705u,          46413368317u },
        {  7556839307242450651u, 14498170692313014398u,         253752925378u },
        { 12946035041643640643u,  1541631360972245751u,         194785947408u },
        { 15885877642352740665u,  9903958882920799117u,          16083572003u },
        { 10770818348246089568u, 15744148547788062576u,          35536894686u },
        { 11635415503599551744u, 17936061801321712000u,         222853492002u },
        {  6248053924100826098u,  9986394078324430610u,          34972315858u },
        { 16894170802729859998u, 13849561248103430369u,         210541363507u },
        {  6143589029651889899u, 12142378807953854930u,          51750786219u },
        { 11812087701837886160u,  2513847703931031444u,         171658239674u },
        {  7306705080150829180u,  1752183758129038045u,         186136275957u },
        {  1485332570280714274u, 15824833342220556540u,         245094986071u },
        { 18042602303295630634u,  8168747198299470695u,          87857865934u },
        { 13397029889257074369u, 17414799840149357478u,         206442828672u },
        {  9948104869613411488u,    83147520704167789u,         128944058191u },
        { 10836066241170646532u,  2383542703041471269u,          79004507436u },
        { 18330574781234459389u, 15540952725549257799u,          44129212108u },
        {   666297360208433062u,  6949835416232048690u,         204842476735u },
        {  5035352224889324309u, 15398868937585367540u,         191376751332u },
        {  1341745796439923765u, 14710915985268256079u,         228834774357u },
        {  6846932182653803785u,  9665704836873335737u,          85797480353u },
        {  6727685027257825533u,  2528789298740305993u,         161523978909u },
        {  2168538874806877737u, 10562914675687726264u,         157137085942u },
        { 17694936100676971444u, 17671658300096837111u,         246572616751u },
        {  2163944241059563294u,   356471401631698552u,          47957982516u },
        {  6727780864524301558u,  7450677157218003204u,          52019324353u },
        {  8599551977795002615u,   317174560787152643u,         193403902018u },
        { 15428264807806859091u,  7251937674440720374u,          66017194067u },
        {  2126771385339683557u,  1252631516699038247u,          83393128329u },
        { 15094316562082972944u, 10818009768860843867u,         137067905290u },
        {  8111300598225956802u, 12330114194950162396u,          10586445484u },
        {  9481215200564260304u, 15826681638261168822u,         172668416829u },
        { 16836674128623424708u, 14240150078499211625u,          61857966130u },
        {  7087243115299722740u, 10725372116242125421u,          50771960082u },
        {  2343560867338408810u,  8434925524647833627u,          18581423587u },
        {  3431385749090422286u, 17133902668520348241u,         227457258228u },
        {  3504545517469224582u, 15093996047981365810u,         244928830724u },
        {  3333759805712094227u,  6187974166976813153u,           4818247165u },
        {  3595183476205994775u, 13946144707720259865u,         253335450751u },
        { 14191566632569921303u,  9138079832881862582u,         127756022019u },
        { 12917427671358095562u,  6600697628576225568u,           3495376300u },
        {  3883793922738316288u,  8137099536646556597u,         172357824535u },
        {  7857281689266421760u, 14169855543453903706u,          23441113049u },
        { 15987081651486195712u,  3706403268650100765u,         217768149408u },
        { 16827562156399525888u, 14736932266877982264u,         160200924523u },
        {  7012737938513461248u, 18004795125138956004u,         107798890698u },
        {  7385903388887613440u,  9068489270661002501u,         202976041899u },
        {                    0u,  7758835715193269217u,         171491603788u },
        {                    0u, 16943947811135261184u,          76420607326u },
        {                    0u,  6745843108403216384u,          94918533251u },
        {                    0u, 12338229654069444608u,         131365692887u },
        {                    0u, 14358176069683511296u,         215668856769u },
        {                    0u,  7083775185760813056u,         193778358284u },
        {                    0u,  5350276357316149248u,          12384012222u },
        {                    0u,  9223372036854775808u,         190290039062u },
        {                    0u,                    0u,          22500000000u },
        { 14135788013842776187u,             13064201u,                    0u },
        { 13284322918167594445u,    13064201766302603u,                    0u },
        {  7264587765474046287u, 14699116688460625612u,               708211u },
        { 16176482219778368741u,  6684126021499623499u,         115796840712u },
        {  5401983818872095469u, 12614606079692508506u,           8362347197u },
        { 11208857446851049921u, 15358270276683001489u,         189683839165u },
        { 13794690008281035639u, 18077126190953408995u,         189832573499u },
        { 14519782740993303071u,  7864121581925945659u,          59979962974u },
        { 10885858587044789123u,  3518026639210514839u,          94426314885u },
        {  8710500938899914621u,  4698310163811252280u,         133190712606u },
        {  1284182587483102819u,  6101155398200416338u,          30254695904u },
        { 10346074482131502030u, 16049178580360033341u,         224330744296u },
        {  9429804686255246574u,  3167464649127375997u,         232870027714u },
        {  6412022633845121254u, 12778923935480989904u,         194171708602u },
        {  8892351297529018260u, 11875553912612980379u,         186692746854u },
        { 14098160105983060597u, 10628760849351697057u,         102643775067u },
        { 13793833029739474340u,  3408944711673234310u,          91576186280u },
        { 16367653765996977044u,  2102091496050506178u,         168184799263u },
        { 10684788343333772342u,  6254611118630245760u,          31113954608u },
        { 13272681218705145345u,  2647941151989776368u,          48339063148u },
        { 11093568615497829248u,  8855437735410157458u,         108143545177u },
        { 12488220765137758124u, 10184270603132180103u,          89480054241u },
        {  3727114642519696453u, 12079083162535627164u,         225552090415u },
        { 11549865375695057514u,  5952952868716156729u,          47654808410u },
        { 11292809154908783229u, 11958907037815852320u,          90322710221u },
        { 17723904861837310998u, 10101562137321697626u,         205648293649u },
        { 14877674388187150875u, 13633527411279258327u,          17547606780u },
        { 12178961950105734308u, 16555627393501768728u,         252739075001u },
        {  4540604068069253114u,  6359650463500280706u,         185897482359u },
        { 13832478722153359868u,  8093923611102181967u,         119344757342u },
        {  1308046668730371491u,  2848827352928635726u,          94438772478u },
        { 18400531023544756800u,  4686723431961561511u,         254154435240u },
        {  9789823458621466539u,  6245554925867652609u,         168254067786u },
        {  1924870562610267306u, 17527406820792516033u,          74338572210u },
        {  5290016144582400923u, 12119966834653692210u,         178950162627u },
        { 15162883663174059077u, 11606502845877928061u,         195657024718u },
        {  8078086116520046390u,   424311496652297090u,         206629189780u },
        { 15731407332173190623u,  5977664048034127173u,         148023001972u },
        {  5346389182763011056u,  6702712461535947028u,         116324049817u },
        {  6368422217216252401u, 11384349854055020018u,         153363354770u },
        {  2961453088119116188u,  3782955013294836188u,         146617146842u },
        { 10932141691610170525u,  3531805968821207061u,         218205074402u },
        { 11034016191361782553u,  3867566898657193228u,         226191459585u },
        {  5455325785621453219u, 12688734637425072080u,           1209661221u },
        {  7735615202566149352u, 18435982764454619691u,          37687857682u },
        {  7502396497775759360u,  4728836163964677692u,          18999416628u },
        {  1601286435751591936u,  2120012917348838977u,          52256350722u },
        { 11449383158571597824u,  9856965465824679831u,           2114926130u },
        { 13043944595690356736u, 11217197671061248816u,          50534347168u },
        {  7773494431818186752u,  3840562972677739189u,         160608085504u },
        {  9943947977234055168u, 17104366978925258617u,            208197335u },
        {                    0u, 16177877219841993444u,         215927229591u },
        {                    0u,  7338522384267208704u,         151877004481u },
        {                    0u, 10935240458612244480u,         193397822095u },
        {                    0u,  1732868046462124032u,         143592800573u },
        {                    0u,   557965042578882560u,          61093938965u },
        {                    0u, 10454684322475540480u,          21030247345u },
        {                    0u, 13907115649320091648u,         177566749572u },
        {                    0u,                    0u,         132753906250u },
        {                    0u,                    0u,          74000000000u },
        {  6343817245135589714u,                  199u,                    0u },
        { 17549323075660516085u,         199343899021u,                    0u },
        {  3948641822109421754u, 14876458284855834550u,                   10u },
        {  1750739713693534543u, 10450704926982265198u,          10806454419u },
        {   962163898128633415u,  5385653213018257806u,         147566533849u },
        {  7967320249386531212u, 12735569669880147489u,         217291956845u },
        {  3018466665533383224u,  3619762560577729456u,         109690396615u },
        { 15076865731854945472u, 11123448126624084269u,         199196227721u },
        {   691187172844604400u,  4072715118852885633u,         137603003331u },
        { 13274492813370992341u, 18239087231420827283u,         195220782328u },
        {  6364168818499152300u,   423431461216085297u,         248988742900u },
        { 17599380787401914158u,  9360976716520160042u,         244022954265u },
        {  3782323149461692814u, 11655927117263208920u,          25507459564u },
        { 14470163442442237466u,  2646622721938364948u,         236631869075u },
        {   476109872130437939u,  4496462484548171852u,         147143473705u },
        { 16330548844673355700u, 13140258519803350063u,          41243753719u },
        { 14089158961463739563u, 13089764333320627770u,         247712334841u },
        { 13385510793074798805u,  6926286827289840501u,         249709597546u },
        {  1549401308746959012u,  4985580225290866218u,         106375474761u },
        {  4607384943843027435u, 10478790837359789693u,          73270268845u },
        {  5157353797716093483u, 10041191967455692214u,         173568056389u },
        { 10846828782671550129u,  5035461258013813797u,          69544334107u },
        {  9915857350819131531u, 14208759661559249750u,          27272972901u },
        {  1673544973504317923u, 12347272163241758840u,         101770258404u },
        { 14780986291622785694u,  3372534174410277614u,         228669346965u },
        {  8168111319515466401u, 17226704187274712984u,         149182825443u },
        { 16345760387859734482u,  4250480179449852121u,         227933861505u },
        {  3177475373321281805u,  4303723537755414374u,         129230418992u },
        {  2558676822419554038u,  8680503847344854165u,          48233305320u },
        {  8813474062662382873u,  8817608623911079652u,         232470571056u },
        {  5426294560236228430u,  5692030448698539450u,          48478003521u },
        {  9919177474128333040u, 16908836314686769809u,          65308565588u },
        {  6197383943089627371u,  6073762347067727240u,          84916629853u },
        { 13673239314867423997u, 10931066692585106200u,          93329259316u },
        {  9309584098968723946u, 14466591364061539596u,          52592574312u },
        {  4647101757759615504u,  4958077340960173341u,         104784235489u },
        {  1405809295505096753u,  4076890037156765715u,         225268777911u },
        { 11332704079573859112u, 14083973146609179058u,         183221008651u },
        {  2960072434514044308u,  2565183738039805295u,          11763493714u },
        {  3887266602785432801u,  1482420938751351224u,          82139058889u },
        { 14545546084687849554u,  2151089495335413944u,         201080362200u },
        {  4617763804182385321u,  3738604531753220913u,         216116610795u },
        {  7857823815580249095u, 14195686514836005765u,         235202670157u },
        { 10939326736548364798u, 17808833916231796970u,          77769549707u },
        { 12262012446566951953u,  1302384553035657354u,         139965418821u },
        { 12555024338687723023u,  1672033517974833698u,          69070602408u },
        {  3332969632922829472u, 11673925532927662545u,         168090641118u },
        { 15535060143360327680u,  3905334232240480709u,         222632844771u },
        { 15794322927987458048u, 17411087320267472625u,         227211708592u },
        { 10571474314433921024u, 16573305231063706617u,         176943856934u },
        { 16679514427547975680u, 15481103236037148354u,          38898440676u },
        { 16925653299565166592u,   907440704754420880u,         228839232288u },
        { 16717361816799281152u,  3224970785139077759u,          32049192459u },
        {                    0u, 10560826509734608144u,          11174826016u },
        {                    0u,  4700940027512659968u,          32572503552u },
        {                    0u,  9733694683502084096u,            254838469u },
        {                    0u,  1995535635724632064u,         197527664646u },
        {                    0u, 10629833226245373952u,           6108178203u },
        {                    0u, 15729384648544878592u,          27576244413u },
        {                    0u,  7205759403792793600u,         189852691650u },
        {                    0u,                    0u,         194390625000u },
        {                    0u,                    0u,         232000000000u },
        {  9335385384027907407u,              3041746u,                    0u },
        { 13237828406194798613u,     3041746506072255u,                    0u },
        { 15667486867836528863u,  7535526066623007027u,               164893u },
        { 17982325043592934313u, 11302146918409311588u,          29408501686u },
        { 17159117626917379189u,  2480833299122194801u,         182612690612u },
        {  8336208968408929657u, 11513226205589330558u,         180134486242u },
        { 12767090573379150201u,  4073957068281936105u,         226624133243u },
        { 14736070002412246709u,  3729887061093812886u,         123220849655u },
        {  9697296975344560756u, 13616911779739451443u,         247202197582u },
        {  7291706381199103298u, 13039053282195777666u,          78738174266u },
        { 18098546597780825068u, 14490756113210417890u,          58706848494u },
        {   132913902678533478u, 17432486112977557585u,         238785545462u },
        {  1879347741692007580u, 14308820825344039837u,         246945016965u },
        {  4056624629214083684u,  4190949538817536349u,         133775682731u },
        { 14713227692042795499u, 13616552502810964397u,         171227191829u },
        {  7366415124022528526u,  4898145803694965031u,          21738154790u },
        {   825770353378039393u,  1399036321001644308u,          38265529016u },
        { 10244023944395357795u, 17170331128243738540u,         184075841910u },
        { 14302658294713551167u, 10641321388205367410u,         118930805515u },
        { 14246653166206862817u,  6648873641312572851u,          11576867188u },
        { 12404486258134291102u,  5988456964560374823u,         116360436162u },
        {  8887442218637942533u,  9972593758348346915u,         194324634902u },
        {  1186724038081863005u, 16709668921872818968u,          22540615390u },
        {   211331772484951576u,  6094829131503407767u,         222905832967u },
        {  6311919513247413649u,  4892016478899926334u,           7330401349u },
        {  8131780018703965703u, 13150857244079031538u,          69265196744u },
        {  2262544347226725725u, 12983943395318785894u,         200712909399u },
        { 15318188749880522583u, 15341644584614757478u,          87703860981u },
        {  1073117094162650652u,  7507635124856644772u,         245831672219u },
        {  4447950380665871747u, 11619655367084544354u,         155406989715u },
        {  5373227185066463609u, 11553116952478783009u,         147629902779u },
        {  6739731406934274193u, 17392150014233193245u,         187626295724u },
        { 12328812617001239444u,  8877887560294980515u,         172942830341u },
        {  3246111484407310759u, 18404180619915609503u,           5481271248u },
        {  3250825415176839770u, 10079413095288181976u,         208997692630u },
        { 16146270540000862342u, 14102802966539105550u,         214546406078u },
        { 15686773375425916830u, 13333966026135891399u,         190764514480u },
        { 11920791905793880226u, 12344968670173516152u,         176722835746u },
        {  1853290561644080707u, 10577007819804726752u,          34669222092u },
        { 12157689141506159076u, 15337041354031088010u,         204573380742u },
        { 18100318838862562546u, 14333607285614673616u,         134831422677u },
        {  7171257882533475139u, 17171597563219696538u,         213777026407u },
        { 14477550873015039462u,  2849642930482147564u,         103930874169u },
        {  8109481182495403274u, 14791248423979435173u,          57154479452u },
        { 14565395719337663965u, 13882371364576310127u,          92801835183u },
        {  1860318978161305991u, 11735995808941329540u,         175752564859u },
        { 16268646275151585618u, 11376996674339273181u,         123636209607u },
        { 13759019338835519104u,  9849638057168043481u,         199616748225u },
        { 17003783176010661888u, 18241520229279361964u,         193533949948u },
        { 18357489540307877888u,  1865852368526961444u,         252988874793u },
        {   905481790074912768u, 10601487369276448158u,          41101148059u },
        {  3638882110636294144u, 15999931310312762170u,         155574707781u },
        {  9011702854368362496u,  5773775867713013570u,          69867358014u },
        { 11529215046068469760u, 17726239863982547534u,          62312997016u },
        {                    0u,  9711316695888316992u,         152960941388u },
        {                    0u, 17872002620723724288u,          76526451532u },
        {                    0u,  7429694208660733952u,          76968843203u },
        {                    0u,  1782821038871019520u,         195402764530u },
        {                    0u,  3225250234313474048u,         242096646922u },
        {                    0u, 10009250171830927360u,          10174841165u },
        {                    0u,  1152921504606846976u,          77542602539u },
        {                    0u,                    0u,          43062500000u },
        {                    0u,                    0u,         160000000000u },
        {  7625299565768063067u,                   46u,                    0u },
        { 13889021769065194705u,          46413368317u,                    0u },
        { 14498170692313014398u,  9519880170333822146u,                    2u },
        {  1541631360972245751u,  2285186318012886800u,           2516073738u },
        {  9903958882920799117u,  9706420951402272035u,          10123880198u },
        { 15744148547788062576u,  2369632031840402142u,           6526186134u },
        { 17936061801321712000u, 15599123897979399458u,         150128458009u },
        {  9986394078324430610u, 17579576584023912658u,          25845630200u },
        { 13849561248103430369u,  3480927339588501811u,         248952990756u },
        { 12142378807953854930u,  3547346616671294635u,          36188701449u },
        {  2513847703931031444u,  7705317123868384954u,           9192302045u },
        {  1752183758129038045u,  4969425237478353909u,         221417706078u },
        { 15824833342220556540u, 17043246700132217175u,          94269393081u },
        {  8168747198299470695u, 17053788362783499470u,         185923916254u },
        { 17414799840149357478u, 11102988228454224768u,         222924487719u },
        {    83147520704167789u, 16944305387801685839u,          39601894197u },
        {  2383542703041471269u, 11725142977459199276u,          53918552635u },
        { 15540952725549257799u,  8175984171998533324u,          59635621274u },
        {  6949835416232048690u,  1372352885142856895u,         154443220990u },
        { 15398868937585367540u, 17975093466502888164u,         254074395398u },
        { 14710915985268256079u,  6467823391459085653u,           6974431769u },
        {  9665704836873335737u, 11319386883146885025u,          25350621408u },
        {  2528789298740305993u,  9141999262922068637u,         224613625192u },
        { 10562914675687726264u,  1587330393383478774u,         104495588773u },
        { 17671658300096837111u,   884187548095712303u,         165086049353u },
        {   356471401631698552u,   488841225726377268u,          73047931903u },
        {  7450677157218003204u, 17462624199405856193u,         255026500135u },
        {   317174560787152643u, 13183677579115583554u,          39946650754u },
        {  7251937674440720374u, 11645015818917277779u,         130714688593u },
        {  1252631516699038247u,  8760523002035971977u,          81631277572u },
        { 10818009768860843867u, 10068817678491468042u,           4474908903u },
        { 12330114194950162396u,  1273658177787418284u,         231545831700u },
        { 15826681638261168822u,  3100019384328057661u,          20069045148u },
        { 14240150078499211625u, 10363063568089458738u,         156168052387u },
        { 10725372116242125421u, 13030756371481403666u,         163561782801u },
        {  8434925524647833627u,  6538878900684195299u,          17706398718u },
        { 17133902668520348241u,  8984884716779098868u,         254354473335u },
        { 15093996047981365810u,  8728727397070363908u,         119487071576u },
        {  6187974166976813153u,  6398650562917867005u,          88473185260u },
        { 13946144707720259865u,  1190873176164938879u,         236346871542u },
        {  9138079832881862582u,  4383628525805121795u,         246064557364u },
        {  6600697628576225568u, 10189374699734119852u,          52237636978u },
        {  8137099536646556597u,  5276291920541626391u,         114552367109u },
        { 14169855543453903706u,  2692252373800386521u,           5286028358u },
        {  3706403268650100765u, 11578684995169173920u,          70145947293u },
        { 14736932266877982264u,  5799408022254132587u,         157627681771u },
        { 18004795125138956004u, 15548569837712345290u,         235314386538u },
        {  9068489270661002501u, 15763030464322902955u,         106842889659u },
        {  7758835715193269217u, 13257749746581255500u,         187854515593u },
        { 16943947811135261184u, 16152470009188707678u,         137718704053u },
        {  6745843108403216384u, 13806790848493904003u,         181875627153u },
        { 12338229654069444608u, 11981226523265951191u,         145748467631u },
        { 14358176069683511296u,  5133628726077003713u,         175649503591u },
        {  7083775185760813056u, 16183955741910833164u,         103278294570u },
        {  5350276357316149248u, 13640425554331371454u,          42877333998u },
        {  9223372036854775808u, 18108120906868035862u,         238739448950u },
        {                    0u,  6324011669895037184u,         118981643201u },
        {                    0u, 10444437689515769856u,         193342825359u },
        {                    0u, 12324712543665782784u,         143566194101u },
        {                    0u, 13928941951563857920u,         181668124005u },
        {                    0u,  3975288688270639104u,         101755089456u },
        {                    0u, 11141905478114607104u,          48215500831u },
        {                    0u,  4611686018427387904u,          31604003906u },
        {                    0u,                    0u,          66250000000u },
        {                    0u,                    0u,         128000000000u },
        { 14699116688460625612u,               708211u,                    0u },
        {  6684126021499623499u,      708211796840712u,                    0u },
        { 12614606079692508506u,  4398362855256705725u,                38392u },
        { 15358270276683001489u,  2812083125569302717u,         248238435728u },
        { 18077126190953408995u, 12868509142973100603u,         144152443331u },
        {  7864121581925945659u,  8726243776748165726u,         195697603278u },
        {  3518026639210514839u,   358304413426858117u,         206473050623u },
        {  4698310163811252280u,  3180720351566429470u,         255019423721u },
        {  6101155398200416338u, 14053818240400098784u,         233172427195u },
        { 16049178580360033341u,  7340140541492429288u,         187761859013u },
        {  3167464649127375997u,  1323571167904965058u,         197397909816u },
        { 12778923935480989904u, 14463851737583396026u,          56071750936u },
        { 11875553912612980379u, 15122784818916048486u,          24784086973u },
        { 10628760849351697057u, 13557974621377508955u,         189819807807u },
        {  3408944711673234310u, 17525172074563876264u,          63734979276u },
        {  2102091496050506178u, 15148880683074215967u,         204950041481u },
        {  6254611118630245760u,  6744828147558597936u,         137821222467u },
        {  2647941151989776368u,  9799290779647971692u,          67365637866u },
        {  8855437735410157458u, 11170890203898678105u,         234531220617u },
        { 10184270603132180103u,  7068779781287527905u,         137605575171u },
        { 12079083162535627164u, 14474741922505540911u,           3383199319u },
        {  5952952868716156729u, 17107062680405191514u,          87784677331u },
        { 11958907037815852320u,  2712598571300237005u,         211927375726u },
        { 10101562137321697626u,  3767556054903418641u,         110147050263u },
        { 13633527411279258327u, 18158239681706277628u,          23204239622u },
        { 16555627393501768728u, 10531652712128330681u,           6984360145u },
        {  6359650463500280706u,  9548395326934120567u,         209570922037u },
        {  8093923611102181967u, 15875647850297719390u,          53517619547u },
        {  2848827352928635726u,  8215825295203192574u,          91860620594u },
        {  4686723431961561511u, 12747310908260543144u,          50445380781u },
        {  6245554925867652609u,    77706528053613642u,         173691033109u },
        { 17527406820792516033u,  6024737704056756146u,          21004212479u },
        { 12119966834653692210u,  6819452388570089667u,         255326601685u },
        { 11606502845877928061u, 13695926775373186254u,         213369683254u },
        {   424311496652297090u,  3746531715392682132u,          54742457678u },
        {  5977664048034127173u,  4717376233154528116u,          78203099891u },
        {  6702712461535947028u,   385190957950313369u,         243255729478u },
        { 11384349854055020018u, 12388374310648616082u,          70020881243u },
        {  3782955013294836188u,  1078067332084407770u,          91671575117u },
        {  3531805968821207061u,  3257295319358714850u,          77058442147u },
        {  3867566898657193228u,  1545453099660723457u,         163176578333u },
        { 12688734637425072080u,  7495477664653506341u,          29083779180u },
        { 18435982764454619691u,  7225503732673614354u,         108406330658u },
        {  4728836163964677692u,  3935478326103643956u,          34391695342u },
        {  2120012917348838977u, 10082240682742686210u,         238213342707u },
        {  9856965465824679831u, 10838712705567897138u,         243546559362u },
        { 11217197671061248816u,  2142546572501643680u,         130587567793u },
        {  3840562972677739189u,  7893042119150331392u,         177116147682u },
        { 17104366978925258617u, 12084811642251302615u,         226427882670u },
        { 16177877219841993444u, 15317234482572954775u,         174655118951u },
        {  7338522384267208704u,  2283226355108359361u,         103830348945u },
        { 10935240458612244480u, 13359725152575722127u,         145123773948u },
        {  1732868046462124032u, 13126551011491594557u,         252724232151u },
        {   557965042578882560u,  3598021288691861269u,         215711591756u },
        { 10454684322475540480u, 16462621795896662961u,          76195049124u },
        { 13907115649320091648u, 14682112756964627332u,         164892440515u },
        {                    0u,  7174112100896070218u,         195795918927u },
        {                    0u,  5023109019590616064u,          79388909396u },
        {                    0u, 10765223023086141440u,          84272303285u },
        {                    0u,  8228137177297453056u,         181583583909u },
        {                    0u,  2891199497780592640u,         165446048210u },
        {                    0u, 15294857653247803392u,         210156732238u },
        {                    0u, 14303432416528695296u,          78829135894u },
        {                    0u,                    0u,          22775390625u },
        {                    0u,                    0u,         161000000000u },
        { 14876458284855834550u,                   10u,                    0u },
        { 10450704926982265198u,          10806454419u,                    0u },
        {  5385653213018257806u, 10806454419566533849u,                    0u },
        { 12735569669880147489u, 17118225092618494573u,            585819067u },
        {  3619762560577729456u, 13385738875341807559u,         187927980841u },
        { 11123448126624084269u,  8272682717439277193u,          41725642358u },
        {  4072715118852885633u, 13402436483369350083u,         118448463028u },
        { 18239087231420827283u, 10946328903241612536u,         180726547537u },
        {   423431461216085297u, 16265808923426731252u,          81593401678u },
        {  9360976716520160042u, 11080374459871185177u,          78881771268u },
        { 11655927117263208920u,  1240761893433831916u,           4600668303u },
        {  2646622721938364948u,   367264070493390483u,         143067261837u },
        {  4496462484548171852u,  2863675693461092905u,         141019909425u },
        { 13140258519803350063u,  7511929581752138999u,          49155240170u },
        { 13089764333320627770u, 11154557789993845753u,         234407222518u },
        {  6926286827289840501u,  8325416539745948522u,         246604689789u },
        {  4985580225290866218u, 17745129874679852617u,         125451321734u },
        { 10478790837359789693u,  1074820986392253357u,         134961965418u },
        { 10041191967455692214u,  7820952682162838597u,         106058266162u },
        {  5035461258013813797u,  8215518006273528603u,          50423974694u },
        { 14208759661559249750u,  9680426791089900133u,          38445364123u },
        { 12347272163241758840u, 16128495723604797412u,         155524776987u },
        {  3372534174410277614u,  2264789053583348885u,          27874327505u },
        { 17226704187274712984u, 11175458488686298083u,         209122774460u },
        {  4250480179449852121u, 11026777810412287617u,         188605822818u },
        {  4303723537755414374u, 16199890034895598640u,          98597762822u },
        {  8680503847344854165u,  9094320719494763752u,           6878197798u },
        {  8817608623911079652u,  1250835564687222832u,          38493004114u },
        {  5692030448698539450u, 15362466642459337025u,          82067807931u },
        { 16908836314686769809u,  7831109835595423828u,         187832800985u },
        {  6073762347067727240u, 15426237284335022429u,         217424525314u },
        { 10931066692585106200u, 15636308361455434548u,           2836257998u },
        { 14466591364061539596u, 13967173875944980328u,         206847645974u },
        {  4958077340960173341u, 18245979923595824097u,          22757162012u },
        {  4076890037156765715u, 11335054479675278263u,          28989116553u },
        { 14083973146609179058u, 11165339882630461707u,         137614474534u },
        {  2565183738039805295u, 15944437408299395922u,          38605274287u },
        {  1482420938751351224u, 15806416348777321161u,         175864349683u },
        {  2151089495335413944u,  4201030477408556248u,         243856867547u },
        {  3738604531753220913u,  9485474942554588907u,         219227738318u },
        { 14195686514836005765u, 18238757647663230541u,         206514208626u },
        { 17808833916231796970u,  4642199687824746379u,         114988725033u },
        {  1302384553035657354u,  6134575894869364037u,          41251654149u },
        {  1672033517974833698u, 11524208547121316008u,           5332556025u },
        { 11673925532927662545u,  2734683241527878366u,         249624728597u },
        {  3905334232240480709u, 10629223456178675171u,          21148247475u },
        { 17411087320267472625u,  2788042336985254064u,         179576211358u },
        { 16573305231063706617u, 17285498758066142502u,         158151140077u },
        { 15481103236037148354u,  5525538192421886436u,         237937048765u },
        {   907440704754420880u, 11414325503043801888u,         189299540025u },
        {  3224970785139077759u,  7246608114685173259u,          57618771825u },
        { 10560826509734608144u,  1007884269852184608u,         113392839413u },
        {  4700940027512659968u, 13823717876510029312u,         245054637515u },
        {  9733694683502084096u, 12487410768239429317u,         203749385247u },
        {  1995535635724632064u,  3361062421598631942u,          31676943894u },
        { 10629833226245373952u, 17853337379088328475u,          22182203558u },
        { 15729384648544878592u, 11551561037491869885u,         166967831358u },
        {  7205759403792793600u, 11480877996635204802u,          62626211378u },
        {                    0u,  5527488381934471912u,          50622379643u },
        {                    0u, 11143438404407726080u,         123299645745u },
        {                    0u,  6472279730688098304u,          49604087006u },
        {                    0u,  4561816853579563008u,         222350862987u },
        {                    0u,  2888714464062865408u,         139247296587u },
        {                    0u, 16258276129784201216u,          75156597524u },
        {                    0u,   720575940379279360u,          20881362915u },
        {                    0u,                    0u,         227039062500u },
        {                    0u,                    0u,         228000000000u },
        {  7535526066623007027u,               164893u,                    0u },
        { 11302146918409311588u,      164893408501686u,                    0u },
        {  2480833299122194801u, 16409970870640346804u,                 8938u },
        { 11513226205589330558u,  7721907286269370594u,         234889586303u },
        {  4073957068281936105u, 14300743897882155131u,         127418605432u },
        {  3729887061093812886u,  2068482633821123575u,         120775244880u },
        { 13616911779739451443u,  4922882895416406094u,          80112132668u },
        { 13039053282195777666u,  9317632875623428410u,          60266870016u },
        { 14490756113210417890u,  5693844901999766254u,            505109890u },
        { 17432486112977557585u, 11569484900262102262u,         130308663950u },
        { 14308820825344039837u,  3138170119352085637u,         142627183033u },
        {  4190949538817536349u,   950584692575235243u,         185170120543u },
        { 13616552502810964397u,  8136430299747162645u,          95051531299u },
        {  4898145803694965031u,  6698711700804594470u,          35441076770u },
        {  1399036321001644308u, 17401191571004302008u,          34363137888u },
        { 17170331128243738540u,  4721732028538188150u,          96943320485u },
        { 10641321388205367410u,  2984214103553086219u,         165255965606u },
        {  6648873641312572851u, 13128675202005662068u,         166161774570u },
        {  5988456964560374823u, 14638512997670672834u,         234711706908u },
        {  9972593758348346915u, 12942085665769692438u,          28793555379u },
        { 16709668921872818968u, 14131134357119205086u,         179701591869u },
        {  6094829131503407767u,  8921946894736102919u,          61766050328u },
        {  4892016478899926334u,  5601522560505809989u,          24483659710u },
        { 13150857244079031538u,  8602606493507716808u,         190303659146u },
        { 12983943395318785894u,  8576789731078566487u,         138466348232u },
        { 15341644584614757478u, 17881118138842658549u,         200464948702u },
        {  7507635124856644772u, 11624372674432704923u,         222969337356u },
        { 11619655367084544354u,  6826284072848095635u,          12630158505u },
        { 11553116952478783009u,  1646466632033733563u,         169370053601u },
        { 17392150014233193245u, 17871081657060299180u,         225089255134u },
        {  8877887560294980515u, 15910893124677544709u,         222968793277u },
        { 18404180619915609503u, 11031217459450580944u,         189862531244u },
        { 10079413095288181976u, 13554987390037243094u,         172598003496u },
        { 14102802966539105550u, 15026714590903687870u,          40734817338u },
        { 13333966026135891399u,  4406379654994689200u,          58814599830u },
        { 12344968670173516152u, 13596329092861950242u,         150238870319u },
        { 10577007819804726752u,   284812388227373260u,          47737058477u },
        { 15337041354031088010u,  9285079159392309382u,         173015439710u },
        { 14333607285614673616u, 15046108141952711893u,          94503345149u },
        { 17171597563219696538u, 13795366909944958311u,         253815651156u },
        {  2849642930482147564u, 12909920641180059961u,          84747848338u },
        { 14791248423979435173u,  5333762939889788252u,         146699848200u },
        { 13882371364576310127u,  6411331390005944495u,           8289143868u },
        { 11735995808941329540u,  1447104583224217723u,          60347558971u },
        { 11376996674339273181u, 11940049226167932871u,          59078447696u },
        {  9849638057168043481u,  9772290783590472385u,          80647271365u },
        { 18241520229279361964u, 16351989577831528444u,         197529756944u },
        {  1865852368526961444u,  4376738725895725097u,          16886443131u },
        { 10601487369276448158u, 13851276297739812763u,         123237263481u },
        { 15999931310312762170u, 12641996203470333509u,         121750879192u },
        {  5773775867713013570u,  7707081716407945022u,         216685323987u },
        { 17726239863982547534u,   417638323657040024u,         211417801737u },
        {  9711316695888316992u, 16438047707692449100u,           9022640218u },
        { 17872002620723724288u, 14850108107043306316u,          90891108351u },
        {  7429694208660733952u, 10423290807904720835u,         255805025973u },
        {  1782821038871019520u, 16951162310302339314u,         181565047726u },
        {  3225250234313474048u,  2752437506572397322u,         174918924350u },
        { 10009250171830927360u,  3925815842962784589u,          62149209936u },
        {  1152921504606846976u,  5274166674003605291u,          80212818903u },
        {                    0u,  5538963350863452832u,         215285913148u },
        {                    0u, 16900671634439028736u,          60300267804u },
        {                    0u,  2326997710751662080u,          28916187245u },
        {                    0u, 12327726161625874432u,         109126146798u },
        {                    0u,  5756455743825903616u,         238668287374u },
        {                    0u,  3018537650245074944u,         142312058091u },
        {                    0u, 16717361816799281152u,         235163635253u },
        {                    0u,                    0u,          53906250000u },
        {                    0u,                    0u,          16000000000u },
        {  2285186318012886800u,           2516073738u,                    0u },
        {  9706420951402272035u,  2516073738123880198u,                    0u },
        {  2369632031840402142u, 11997425759292732054u,            136396630u },
        { 15599123897979399458u, 11491152661270395161u,          86650381753u },
        { 17579576584023912658u, 18181063258234881272u,         185622936633u },
        {  3480927339588501811u,  2466921813123869732u,          57985597414u },
        {  3547346616671294635u,  8430880678232179465u,         230133732099u },
        {  7705317123868384954u,  6738034873677997533u,           3457038957u },
        {  4969425237478353909u,  7678250951042929246u,         109365269602u },
        { 17043246700132217175u,  1853560606315563193u,          98416238818u },
        { 17053788362783499470u, 14942676593409905118u,         226100481721u },
        { 11102988228454224768u,  4909892170837638183u,         185810044121u },
        { 16944305387801685839u, 16871149368312132405u,         217266165787u },
        { 11725142977459199276u, 16096130589333770811u,          27914586839u },
        {  8175984171998533324u, 12512479187631824282u,         215872572987u },
        {  1372352885142856895u, 16980304980540557310u,          59678302855u },
        { 17975093466502888164u,  8640919162749295366u,         135920504177u },
        {  6467823391459085653u,  7862382415464063513u,         113468425166u },
        { 11319386883146885025u, 14534157903009925344u,         206426220604u },
        {  9141999262922068637u, 12627464554215107944u,          60787898278u },
        {  1587330393383478774u,  2456849734836299173u,         166684536225u },
        {   884187548095712303u, 18428252197697827913u,         161133186090u },
        {   488841225726377268u,  7244734215936736255u,          42998997553u },
        { 17462624199405856193u, 14756175050504770087u,          49392737828u },
        { 13183677579115583554u,  6764116534566945922u,          36799933852u },
        { 11645015818917277779u,  1588822142405565521u,         156366683492u },
        {  8760523002035971977u, 17053265624843842052u,         100086130220u },
        { 10068817678491468042u, 16996891591759999207u,          44924459381u },
        {  1273658177787418284u,  8565556232370585876u,         117921403339u },
        {  3100019384328057661u, 14464960359145886620u,         203464339733u },
        { 10363063568089458738u,  5813189542048784035u,          21784147072u },
        { 13030756371481403666u,  9739241026882027025u,         128315133636u },
        {  6538878900684195299u, 18175068535675302910u,         196527965313u },
        {  8984884716779098868u, 10562697212061761911u,         129985272439u },
        {  8728727397070363908u,  4264834835660801368u,         119572604963u },
        {  6398650562917867005u, 13019066443690126316u,          35231197159u },
        {  1190873176164938879u,  1828040177823321846u,         231705765006u },
        {  4383628525805121795u, 11240369830376975668u,         142099098256u },
        { 10189374699734119852u,  8886938465302549874u,         144609341669u },
        {  5276291920541626391u,  9985240313589688325u,         229481761899u },
        {  2692252373800386521u,   722909126956573766u,         107541300962u },
        { 11578684995169173920u,  5493363474638452381u,         226039188982u },
        {  5799408022254132587u, 12410535279213120491u,         246297795830u },
        { 15548569837712345290u, 10543108918366869098u,         246672776465u },
        { 15763030464322902955u, 12953909016524823995u,          17571543079u },
        { 13257749746581255500u, 16505942145872588169u,          39702232814u },
        { 16152470009188707678u, 12428594380392015797u,         238894788916u },
        { 13806790848493904003u,  7528259605829768337u,          52673755451u },
        { 11981226523265951191u, 18147447600042811311u,          59408107770u },
        {  5133628726077003713u, 12021069431116183911u,         250983775105u },
        { 16183955741910833164u, 11819985069665662506u,         129651663479u },
        { 13640425554331371454u, 10401877114068152814u,         119640762674u },
        { 18108120906868035862u,  4611631138117837942u,          50563886888u },
        {  6324011669895037184u, 17200813398607252417u,          40249997024u },
        { 10444437689515769856u, 14100466137553658767u,         224932457962u },
        { 12324712543665782784u, 17887776768825509301u,         234764387800u },
        { 13928941951563857920u, 12632656857970087269u,         216969698321u },
        {  3975288688270639104u,  8923681664054686256u,          17684817700u },
        { 11141905478114607104u,  6213926103737837599u,          36483753752u },
        {  4611686018427387904u,  1233118281776157762u,          24336857609u },
        {                    0u,    30716279628678784u,           9066847476u },
        {                    0u, 15775734650898546688u,         244001665132u },
        {                    0u,   976806005729918976u,         108855204289u },
        {                    0u, 12460098853279891456u,         193052952759u },
        {                    0u,  5635665595421687808u,         183675463312u },
        {                    0u,  1805943450575568896u,         144305510044u },
        {                    0u, 11529215046068469760u,         156097900390u },
        {                    0u,                    0u,         102625000000u },
        {                    0u,                    0u,          64000000000u },
        {  4398362855256705725u,                38392u,                    0u },
        {  2812083125569302717u,       38392238435728u,                    0u },
        { 12868509142973100603u,  4564018338575530435u,                 2081u },
        {  8726243776748165726u, 16553437246451512014u,          33247415929u },
        {   358304413426858117u,  4339777136957372927u,         121897363631u },
        {  3180720351566429470u, 18439463366554654697u,         175235259789u },
        { 14053818240400098784u,  1370067356680643003u,         141999605312u },
        {  7340140541492429288u,  4210124040914115013u,          64074271500u },
        {  1323571167904965058u, 10692225626142609720u,          12228231281u },
        { 14463851737583396026u, 11592856673895384344u,         113579626712u },
        { 15122784818916048486u, 10284479231227406269u,         216628450019u },
        { 13557974621377508955u,  4961071383534266431u,         227557522736u },
        { 17525172074563876264u, 10960611551445686988u,          48268940218u },
        { 15148880683074215967u, 14616396723115619209u,         186594175942u },
        {  6744828147558597936u,  1025604265437492803u,         198792356454u },
        {  9799290779647971692u, 11711588454892179178u,         102055598118u },
        { 11170890203898678105u,  5580373263251565705u,          38634886482u },
        {  7068779781287527905u, 14109334653033148931u,          82302512640u },
        { 14474741922505540911u,  2899414033769399895u,            764868564u },
        { 17107062680405191514u, 13233457234892808147u,         212157177549u },
        {  2712598571300237005u,  3287946691509034862u,         205717387154u },
        {  3767556054903418641u,  5488480288717445911u,         146178239947u },
        { 18158239681706277628u, 11687233053874362630u,         203297531112u },
        { 10531652712128330681u,  6783772100089274577u,         232633566173u },
        {  9548395326934120567u,  7898291058728402485u,         221367749022u },
        { 15875647850297719390u,  4423684977486598491u,         158428167216u },
        {  8215825295203192574u,  2750833684599526706u,          48239808443u },
        { 12747310908260543144u, 15669689830489025709u,         187149122992u },
        {    77706528053613642u, 15117307274214954517u,         176849455587u },
        {  6024737704056756146u,  8148639818575698175u,         227819510869u },
        {  6819452388570089667u, 13006484426078994901u,          85441738649u },
        { 13695926775373186254u, 10287496057845513526u,         153705082933u },
        {  3746531715392682132u, 14159876032966532430u,          53557686278u },
        {  4717376233154528116u, 15742212196465548019u,           6767608417u },
        {   385190957950313369u,  2892220461917134150u,          97853387033u },
        { 12388374310648616082u,  7487151560715393883u,          25156787585u },
        {  1078067332084407770u,  7245756744165177933u,         129405879299u },
        {  3257295319358714850u,  3067122860671533987u,           3392793260u },
        {  1545453099660723457u,  8135043905834122525u,         172166269063u },
        {  7495477664653506341u, 14730019368921022572u,         135441001613u },
        {  7225503732673614354u,   495969939682055458u,         141798515950u },
        {  3935478326103643956u,  5617761407265775598u,         238026886584u },
        { 10082240682742686210u,  2087044847072781811u,         184304539456u },
        { 10838712705567897138u, 15929674232061203330u,          64113138927u },
        {  2142546572501643680u,  8658086469608285873u,         239863549370u },
        {  7893042119150331392u, 18369871790780313570u,         186469355807u },
        { 12084811642251302615u,  3545648451947416750u,          31995832745u },
        { 15317234482572954775u, 13347376792767929959u,         169192209987u },
        {  2283226355108359361u, 14482164459838203025u,          67723562745u },
        { 13359725152575722127u,  8899577765623565820u,         249785079708u },
        { 13126551011491594557u,  7095320096604405719u,         156482447077u },
        {  3598021288691861269u,  2968593824439315788u,         229384638073u },
        { 16462621795896662961u, 12621408323612585636u,         121160927793u },
        { 14682112756964627332u,  3954422936414648259u,          49684207916u },
        {  7174112100896070218u, 17143730087577690191u,          44214369696u },
        {  5023109019590616064u,  5033045529399041876u,         160929363470u },
        { 10765223023086141440u, 15857648521994521781u,          14272841944u },
        {  8228137177297453056u, 16655573486499109541u,         216859644848u },
        {  2891199497780592640u, 16652154439190075858u,         176902900447u },
        { 15294857653247803392u, 18016950600164130638u,         223902715100u },
        { 14303432416528695296u,  2086292996072613910u,         220976700849u },
        {                    0u, 17324462585194799521u,         177113098169u },
        {                    0u, 11079151463184927232u,         185939160998u },
        {                    0u,  5239846817488961536u,         166600602004u },
        {                    0u,  2778806963520143360u,         148284052665u },
        {                    0u,  6240890740138835968u,         185150639427u },
        {                    0u, 17250651344549707776u,          67338319364u },
        {                    0u,  4197354852709302272u,           4935159683u },
        {                    0u,  9223372036854775808u,         131227539062u },
        {                    0u,                    0u,         118500000000u },
        { 17118225092618494573u,            585819067u,                    0u },
        { 13385738875341807559u,   585819067927980841u,                    0u },
        {  8272682717439277193u,  5654803392547571318u,             31757315u },
        { 13402436483369350083u,  2931628102185393332u,           3306547506u },
        { 10946328903241612536u, 15964697617980212305u,          50158923877u },
        { 16265808923426731252u,   450380868305846606u,         101865447992u },
        { 11080374459871185177u, 14631133530814566148u,          56024415195u },
        {  1240761893433831916u,    31969822783742095u,         219793155338u },
        {   367264070493390483u, 10437269029385743245u,          10001733087u },
        {  2863675693461092905u, 15196146496377392433u,         223565805487u },
        {  7511929581752138999u,  4409099735137480938u,         175823784752u },
        { 11154557789993845753u, 10644987914903248118u,          48239017775u },
        {  8325416539745948522u,  3154431617534062973u,          47577065951u },
        { 17745129874679852617u, 11702056331247960454u,         223171002080u },
        {  1074820986392253357u, 15575315065965259114u,         224634369744u },
        {  7820952682162838597u, 10759747609480050226u,         208844339521u },
        {  8215518006273528603u, 12538236653960743718u,          65583287086u },
        {  9680426791089900133u, 17857942663978005403u,          46679699170u },
        { 16128495723604797412u, 11443004154750813211u,         226968081011u },
        {  2264789053583348885u,  4004313188770806737u,         115620326498u },
        { 11175458488686298083u, 17134872954824183228u,          98217074252u },
        { 11026777810412287617u,  2659553912986171234u,          76928883324u },
        { 16199890034895598640u,  9501854300969137926u,         124144174706u },
        {  9094320719494763752u, 14528169966301018150u,         114515096553u },
        {  1250835564687222832u, 18172091996515901778u,         233787573671u },
        { 15362466642459337025u,  1133541705604751035u,         167985111081u },
        {  7831109835595423828u, 18280349987988641497u,          41061449418u },
        { 15426237284335022429u,  9936015874712336386u,         202990979758u },
        { 15636308361455434548u, 15876720399740689614u,         174538632499u },
        { 13967173875944980328u,  8618117825152456982u,          51860678737u },
        { 18245979923595824097u,  8085525680745921564u,          81467189103u },
        { 11335054479675278263u,  8072355444669730953u,         111438317225u },
        { 11165339882630461707u,  9395030504766848294u,         169437603265u },
        { 15944437408299395922u,  3537903114058185903u,         193509305624u },
        { 15806416348777321161u,  2126094743961928691u,          24191790112u },
        {  4201030477408556248u,   289185362555601115u,          32115255827u },
        {  9485474942554588907u, 16909937501450129614u,          19015676769u },
        { 18238757647663230541u, 14449642060360499058u,          97916689548u },
        {  4642199687824746379u, 12433818908498244393u,         140783316665u },
        {  6134575894869364037u, 11884444034578008581u,         185674038673u },
        { 11524208547121316008u,   988625838444140793u,         145644257002u },
        {  2734683241527878366u,  1675370907158909973u,         234053593514u },
        { 10629223456178675171u, 15920186275316934067u,         170090822038u },
        {  2788042336985254064u,  5600921198503757726u,         150863035027u },
        { 17285498758066142502u, 10457357161776341741u,         147303626546u },
        {  5525538192421886436u, 12225356765775740093u,          50566894467u },
        { 11414325503043801888u,  4486633318598164537u,         131662737918u },
        {  7246608114685173259u, 10302486602879381361u,         254243220879u },
        {  1007884269852184608u, 15536428611301239541u,         143558498917u },
        { 13823717876510029312u, 12026126645955462603u,         101842231482u },
        { 12487410768239429317u, 14877968141142123551u,         186651937631u },
        {  3361062421598631942u,   734560801645383190u,          95806536269u },
        { 17853337379088328475u, 15648943144911081638u,          77039820620u },
        { 11551561037491869885u, 13664182862003235646u,          76848330907u },
        { 11480877996635204802u,  3895127525902132786u,         155740736837u },
        {  5527488381934471912u,  5249187334214137467u,          69211155286u },
        { 11143438404407726080u, 10642260063359027505u,          86284559015u },
        {  6472279730688098304u,   783598951897779422u,         167576918074u },
        {  4561816853579563008u,  5538576558607624843u,          58042478984u },
        {  2888714464062865408u, 15974581187564609611u,         136300246836u },
        { 16258276129784201216u,  7474269406918257428u,          52865983781u },
        {   720575940379279360u,  8045286838779138019u,          37405180956u },
        {                    0u,  8184246376556341732u,          28436135873u },
        {                    0u,  1493267152679331840u,         193443668885u },
        {                    0u, 10179074811222818816u,         149080950174u },
        {                    0u,  3892499202005008384u,         158551808751u },
        {                    0u, 10341173215925108736u,         239211012804u },
        {                    0u,  6230307872002015232u,         196560596123u },
        {                    0u,  9295429630892703744u,         155337745666u },
        {                    0u,                    0u,           2503906250u },
        {                    0u,                    0u,         202000000000u },
        { 16409970870640346804u,                 8938u,                    0u },
        {  7721907286269370594u,        8938889586303u,                    0u },
        { 14300743897882155131u, 10665454627995623288u,                  484u },
        {  2068482633821123575u, 16803537892767562832u,         228578175453u },
        {  4922882895416406094u,  8099123106849104444u,         221910921614u },
        {  9317632875623428410u,  7077413686679401728u,         142439054343u },
        {  5693844901999766254u, 13536636358372449666u,           7383667364u },
        { 11569484900262102262u,  7280632235418610318u,         164733822527u },
        {  3138170119352085637u,  6187823673116858809u,          63394683864u },
        {   950584692575235243u,  8624343686231740255u,         216335442593u },
        {  8136430299747162645u,   806211610822132771u,         161467526608u },
        {  6698711700804594470u, 18388078233202190882u,         208043704818u },
        { 17401191571004302008u,  7628864426595573600u,         242996819718u },
        {  4721732028538188150u,  4530799784343874981u,           6413561569u },
        {  2984214103553086219u,  8561580552078486438u,         225245615148u },
        { 13128675202005662068u, 13349114951221999594u,          44464124211u },
        { 14638512997670672834u, 10029144738508991772u,          51723656971u },
        { 12942085665769692438u, 12601907197916268979u,          11543681025u },
        { 14131134357119205086u,  1329580921391066941u,           1683150758u },
        {  8921946894736102919u,  3198179786356761112u,         166072076726u },
        {  5601522560505809989u, 11406753413634654142u,         182173373673u },
        {  8602606493507716808u, 11131812960525182090u,         233618361341u },
        {  8576789731078566487u, 14299636753645227208u,         253603456789u },
        { 17881118138842658549u, 12964114684643663326u,          21775184861u },
        { 11624372674432704923u,  5019257593846306316u,         221702786065u },
        {  6826284072848095635u,  6929086798159998121u,          17272094499u },
        {  1646466632033733563u, 18359765766933703649u,          35375626547u },
        { 17871081657060299180u,  9993076234752063198u,          51995284896u },
        { 15910893124677544709u,  3257189215046584509u,         160541725748u },
        { 11031217459450580944u,  2905234736672690348u,          52176572581u },
        { 13554987390037243094u, 12064985302079670056u,         165157493090u },
        { 15026714590903687870u, 14315096064942799930u,          98654044163u },
        {  4406379654994689200u, 11943971043551974038u,           3776022912u },
        { 13596329092861950242u, 12472773152119929647u,         128647483967u },
        {   284812388227373260u,  7791259796982183085u,          63676150387u },
        {  9285079159392309382u, 16866829442051086686u,         115422365039u },
        { 15046108141952711893u,  3702498393844653053u,         111914352656u },
        { 13795366909944958311u,  2057239613841701716u,          16200712840u },
        { 12909920641180059961u, 17201969976738286226u,         136111523182u },
        {  5333762939889788252u, 18271566505443461640u,         110932520660u },
        {  6411331390005944495u, 18368509115417119804u,         212990503604u },
        {  1447104583224217723u,  7613923684154518587u,         180995758874u },
        { 11940049226167932871u, 17984805084714865232u,          26412751629u },
        {  9772290783590472385u,  4220802739051410373u,          13974958237u },
        { 16351989577831528444u, 17812459042810815760u,         157228810174u },
        {  4376738725895725097u, 10629526089664605307u,         190965615339u },
        { 13851276297739812763u, 17437443267816548473u,         235576227763u },
        { 12641996203470333509u, 12506371893701049304u,         179945285693u },
        {  7707081716407945022u, 15737221540003030739u,          61677971778u },
        {   417638323657040024u,  2358380859011605513u,          66853116489u },
        { 16438047707692449100u, 10042972713837039706u,          73127848082u },
        { 14850108107043306316u, 13424397272769642495u,         146544430641u },
        { 10423290807904720835u,  6867102315755663029u,          49727738034u },
        { 16951162310302339314u,  8690748404825506734u,         178372266362u },
        {  2752437506572397322u,   956229930815387710u,         122471126415u },
        {  3925815842962784589u,  7734449506297687888u,         143051837328u },
        {  5274166674003605291u, 16332184961683848151u,         144419285347u },
        {  5538963350863452832u, 15580777817612768828u,          99885369520u },
        { 16900671634439028736u, 17404245271944696092u,         176844635657u },
        {  2326997710751662080u, 13201420160494469229u,           9943486026u },
        { 12327726161625874432u, 16511717657124068078u,          74715650420u },
        {  5756455743825903616u, 14131292492116594062u,         116895102007u },
        {  3018537650245074944u, 18429136031865875691u,          55766058900u },
        { 16717361816799281152u,  2563978348305862197u,         148999045466u },
        {                    0u, 14239974392147482896u,          90138993544u },
        {                    0u, 11164201396098998272u,         136771950558u },
        {                    0u,  7116971104932986880u,         222605212570u },
        {                    0u, 12437629862867369984u,         154385811776u },
        {                    0u, 16501893821638901760u,          64674245265u },
        {                    0u, 10649324268870959104u,         145894569456u },
        {                    0u,  7205759403792793600u,         240577301025u },
        {                    0u,                    0u,          33390625000u },
        {                    0u,                    0u,         232000000000u },
        { 11997425759292732054u,            136396630u,                    0u },
        { 11491152661270395161u,   136396630650381753u,                    0u },
        { 18181063258234881272u,  3016823727048309817u,              7394076u },
        {  2466921813123869732u, 17405973192644624358u,          28163542341u },
        {  8430880678232179465u,  8937219978302591747u,          69943579697u },
        {  6738034873677997533u, 15178463196824222317u,          49484487665u },
        {  7678250951042929246u, 11979404627460330594u,         241822826138u },
        {  1853560606315563193u,  2006448052689740002u,         154649404826u },
        { 14942676593409905118u, 16330465320863239865u,         154108769766u },
        {  4909892170837638183u, 17136208883957646553u,         230885276298u },
        { 16871149368312132405u,   140455118208931867u,         138928955745u },
        { 16096130589333770811u,  3964972929179372247u,          97007614087u },
        { 12512479187631824282u,  3378050330022776379u,         135214941613u },
        { 16980304980540557310u,  6065353437512901255u,         173183124475u },
        {  8640919162749295366u, 12768753059854699889u,         251328803468u },
        {  7862382415464063513u,  6848720690951013326u,         140692195490u },
        { 14534157903009925344u, 10953228058585475132u,         162371269892u },
        { 12627464554215107944u, 15539127852083296166u,           4593775682u },
        {  2456849734836299173u, 14534853647735598497u,          66842377808u },
        { 18428252197697827913u,  1506909603576368170u,          80787935995u },
        {  7244734215936736255u,  5475702579938239025u,         251081689733u },
        { 14756175050504770087u, 12039747373985783332u,         133296838431u },
        {  6764116534566945922u, 17572399137760898460u,          31652676012u },
        {  1588822142405565521u,   869552790852091236u,         172952601666u },
        { 17053265624843842052u,  4549585778048181804u,          66047138551u },
        { 16996891591759999207u,  4121918231767210357u,         247246633539u },
        {  8565556232370585876u,  1558397953312543179u,          67223449635u },
        { 14464960359145886620u,  6067524298738069781u,          35084480922u },
        {  5813189542048784035u,  5811095224555517056u,         154328921151u },
        {  9739241026882027025u,  6440894514158997188u,          63315020103u },
        { 18175068535675302910u,  4612748874388784257u,          71349161591u },
        { 10562697212061761911u,  9908101430749813367u,         119250057617u },
        {  4264834835660801368u, 15150017990912190499u,         145537119254u },
        { 13019066443690126316u, 17470426264690059239u,          22821284120u },
        {  1828040177823321846u,  9615161096851907726u,          24947073705u },
        { 11240369830376975668u,  9227932132124142224u,         169521238927u },
        {  8886938465302549874u,  4794113194321211621u,         143500247203u },
        {  9985240313589688325u,   391512698859146347u,         163259889397u },
        {   722909126956573766u, 17209658878068655842u,         245021223945u },
        {  5493363474638452381u,  3077364726606876150u,           9932937477u },
        { 12410535279213120491u,  1952989567673965814u,           5166824276u },
        { 10543108918366869098u, 11172860676923186449u,          84105871776u },
        { 12953909016524823995u, 17338078544784947239u,         160605681990u },
        { 16505942145872588169u,  4593380466519703278u,          70939899121u },
        { 12428594380392015797u,   786884753602720052u,         241249007654u },
        {  7528259605829768337u, 17848875822468020539u,          38042657107u },
        { 18147447600042811311u,  2899664567187130618u,          83967589497u },
        { 12021069431116183911u,  2973178834961857409u,         121157191131u },
        { 11819985069665662506u, 11117453141176836727u,         219161176347u },
        { 10401877114068152814u,  7535238370146462002u,          27602678342u },
        {  4611631138117837942u, 10246175467290865448u,          70408486090u },
        { 17200813398607252417u,  1203128834127050464u,         202555446285u },
        { 14100466137553658767u, 14518048959078919658u,          13065221744u },
        { 17887776768825509301u,  1553474987376920024u,         112787025011u },
        { 12632656857970087269u, 14956572380830948369u,         115084214047u },
        {  8923681664054686256u,  7594162606042048292u,          31810797413u },
        {  6213926103737837599u, 14461296147288811288u,         101411680379u },
        {  1233118281776157762u, 18305427728131488265u,         123783948434u },
        {    30716279628678784u, 10253208939347909876u,         146992339225u },
        { 15775734650898546688u,  6446028915490812012u,          25555827570u },
        {   976806005729918976u, 12986063676957432257u,         114349439927u },
        { 12460098853279891456u,  9769714697972762807u,         183703975922u },
        {  5635665595421687808u,    97429465146664592u,         242529617295u },
        {  1805943450575568896u, 16395571728207795868u,         143005281661u },
        { 11529215046068469760u,  6331668478323650406u,         125888805724u },
        {                    0u, 18129911846294207040u,          92343240435u },
        {                    0u,  9890094564876124160u,         243982824490u },
        {                    0u, 12290856656987750400u,          42536143100u },
        {                    0u,  8498454992640802816u,         252666288674u },
        {                    0u,  5341660584200896512u,          34460702168u },
        {                    0u,  9288674231451648000u,         216289572000u },
        {                    0u,  1152921504606846976u,         160503540039u },
        {                    0u,                    0u,          71062500000u },
        {                    0u,                    0u,         160000000000u },
        {  4564018338575530435u,                 2081u,                    0u },
        { 16553437246451512014u,        2081247415929u,                    0u },
        {  4339777136957372927u, 15212079674427582639u,                  112u },
        { 18439463366554654697u, 10179808126814248333u,         112824648491u },
        {  1370067356680643003u,  6066766544199222848u,          43551848504u },
        {  4210124040914115013u,  6625308131806923532u,          56328880073u },
        { 10692225626142609720u,  9122786786400665713u,         201359158673u },
        { 11592856673895384344u, 11932880778639151320u,         145494547262u },
        { 10284479231227406269u,  3884040911779255011u,          62646882763u },
        {  4961071383534266431u, 13441817515637357872u,         203210554279u },
        { 10960611551445686988u, 11628577856022352826u,         167728682387u },
        { 14616396723115619209u, 13296656925520243654u,         147630386468u },
        {  1025604265437492803u,  5020720704545399398u,          36720813216u },
        { 11711588454892179178u, 14121973606499014694u,         160272173814u },
        {  5580373263251565705u,  3642481034345420114u,         246765553723u },
        { 14109334653033148931u,  9845536238569696768u,          59197459292u },
        {  2899414033769399895u, 17655403572195686356u,          92533727588u },
        { 13233457234892808147u,  8377495365136654029u,         100957101345u },
        {  3287946691509034862u, 13713682649609025426u,          33454144933u },
        {  5488480288717445911u,  1367709905452854731u,         165743420226u },
        { 11687233053874362630u,  9981467701727208680u,          66074143702u },
        {  6783772100089274577u,  6277920117543306205u,         214541096448u },
        {  7898291058728402485u,  9344111460418701726u,            340326731u },
        {  4423684977486598491u,  4918507011364617264u,          75506545297u },
        {  2750833684599526706u,  6554777203830755259u,         145266632799u },
        { 15669689830489025709u,  4198262173120265648u,          95355335184u },
        { 15117307274214954517u,  8080325935698446819u,          16227588248u },
        {  8148639818575698175u, 12797633874200091733u,         152438035346u },
        { 13006484426078994901u,  8376502502208665497u,         146693761122u },
        { 10287496057845513526u,  9891973386793349173u,          98454091110u },
        { 14159876032966532430u, 14877430279003795462u,         102536244951u },
        { 15742212196465548019u,  8759933935842067041u,         215806507111u },
        {  2892220461917134150u,  3753418510388703513u,         103474876970u },
        {  7487151560715393883u,  2961383332545305985u,          42203473225u },
        {  7245756744165177933u,  2497674184068629507u,          73160536912u },
        {  3067122860671533987u, 15244544070742305452u,          80135399188u },
        {  8135043905834122525u,    45953573565810823u,          20826408390u },
        { 14730019368921022572u,  3960077421351906445u,         198002491148u },
        {   495969939682055458u,  3173330011013883118u,          12214676227u },
        {  5617761407265775598u, 11026266219545759160u,           3172026564u },
        {  2087044847072781811u,  8886757764964685632u,         196597735089u },
        { 15929674232061203330u, 13952322129918090479u,         177481752103u },
        {  8658086469608285873u,  4127250666614902202u,          39756356898u },
        { 18369871790780313570u, 17649958504065306911u,          34223738706u },
        {  3545648451947416750u, 13269305359002216873u,          82956806167u },
        { 13347376792767929959u, 16236593433831947843u,          23719330484u },
        { 14482164459838203025u, 13580930396682424057u,         180880187493u },
        {  8899577765623565820u,   421976357197961116u,         101736223712u },
        {  7095320096604405719u,  2962130818798626533u,         224022875384u },
        {  2968593824439315788u,  8234383947306356345u,         248160577433u },
        { 12621408323612585636u,  4380469931801381425u,         153446386848u },
        {  3954422936414648259u, 15279887469027055916u,         160237465750u },
        { 17143730087577690191u,  8534542821713755552u,         150828324359u },
        {  5033045529399041876u,  7814613482565088782u,           7462658493u },
        { 15857648521994521781u, 13771954404705323224u,         189423631045u },
        { 16655573486499109541u,  4568173274762548144u,         197746579144u },
        { 16652154439190075858u,  8105292616250821343u,         200247641169u },
        { 18016950600164130638u,  2923678426777275612u,          81439388793u },
        {  2086292996072613910u,  1808633176918384049u,         121158492925u },
        { 17324462585194799521u, 18118642609460438969u,         253098046200u },
        { 11079151463184927232u, 18138164175864360870u,         248982213583u },
        {  5239846817488961536u,  4031433690465792404u,         207983271850u },
        {  2778806963520143360u,  5012226396942308537u,         170218544458u },
        {  6240890740138835968u,  7889712298793536835u,          74271713337u },
        { 17250651344549707776u, 13500762396543628804u,          57427702160u },
        {  4197354852709302272u,   501020624068841347u,         144731877796u },
        {  9223372036854775808u,  8370653768288261750u,         164027160382u },
        {                    0u,   647579990023635200u,          62453774050u },
        {                    0u, 11106569307181154304u,         226035105381u },
        {                    0u, 10797461613892861952u,         101602088328u },
        {                    0u, 17627230675448889344u,         136585331566u },
        {                    0u, 12197735707942322176u,         110955574089u },
        {                    0u, 12871287735024877568u,          73661240577u },
        {                    0u,  4611686018427387904u,           1697753906u },
        {                    0u,                    0u,          50250000000u },
        {                    0u,                    0u,         128000000000u },
        {  5654803392547571318u,             31757315u,                    0u },
        {  2931628102185393332u,    31757315306547506u,                    0u },
        { 15964697617980212305u,  9451803574512021605u,              1721567u },
        {   450380868305846606u,  8662766454758138424u,         223512383298u },
        { 14631133530814566148u,  9207992007314947035u,          66469609510u },
        {    31969822783742095u, 17118602861291201802u,          38499166246u },
        { 10437269029385743245u, 11186560605745599967u,          38928001320u },
        { 15196146496377392433u, 10505549821532796847u,          40606424665u },
        {  4409099735137480938u, 18133667530488679216u,          89569506996u },
        { 10644987914903248118u, 10778135771244330799u,         180983028086u },
        {  3154431617534062973u, 17087985777033767391u,         118584283910u },
        { 11702056331247960454u,  2639185991757283040u,           6926341565u },
        { 15575315065965259114u,  5401720287293896400u,         189143070559u },
        { 10759747609480050226u,  9816495392633895233u,          95292827843u },
        { 12538236653960743718u, 10042051500090034990u,         195532153281u },
        { 17857942663978005403u, 11629689537856384738u,         193544380702u },
        { 11443004154750813211u,  2099086731766010483u,          30630446733u },
        {  4004313188770806737u, 13665537898516458594u,         141113791719u },
        { 17134872954824183228u, 16375672064669490764u,         231740810293u },
        {  2659553912986171234u,  7770550512184564348u,          53887726961u },
        {  9501854300969137926u,  6197048880720627314u,         113421242387u },
        { 14528169966301018150u, 17963594118523106281u,          19335942692u },
        { 18172091996515901778u,  8255454642407818663u,          36973808388u },
        {  1133541705604751035u, 16744201957549498409u,           4447529092u },
        { 18280349987988641497u, 17442505417202859722u,         132907705006u },
        {  9936015874712336386u,  6383975767786687150u,         174945560113u },
        { 15876720399740689614u, 15245442964998335795u,          49346076019u },
        {  8618117825152456982u,  2910016124519524433u,         115826457119u },
        {  8085525680745921564u,  3847913871169988463u,          31157752290u },
        {  8072355444669730953u, 17210451512590059177u,         226208595828u },
        {  9395030504766848294u, 17899408909991454145u,         116932980445u },
        {  3537903114058185903u,  5920601932753251608u,         221970328901u },
        {  2126094743961928691u, 16521781895108979744u,          69320956473u },
        {   289185362555601115u,  3697493405554698771u,          57895647591u },
        { 16909937501450129614u,  2816108280295732065u,         103200441519u },
        { 14449642060360499058u, 14251078772056398988u,         175152661535u },
        { 12433818908498244393u,  4543066550096031417u,          31772552528u },
        { 11884444034578008581u,  3099369389734296977u,          80246280131u },
        {   988625838444140793u,  5243484113636490986u,         195168017151u },
        {  1675370907158909973u,  6823370511605197226u,         255284249843u },
        { 15920186275316934067u, 11396290277624641942u,         243369895656u },
        {  5600921198503757726u, 15934361408437566099u,         232617794133u },
        { 10457357161776341741u, 14939272230935131954u,          85863803462u },
        { 12225356765775740093u,  7500666177940329347u,          70809859570u },
        {  4486633318598164537u,  4806714453065462270u,         242406611928u },
        { 10302486602879381361u, 11557851247268441487u,         216260572512u },
        { 15536428611301239541u, 10655523157206817381u,          96626552371u },
        { 12026126645955462603u, 14769600176490881210u,          51577637067u },
        { 14877968141142123551u, 16688495540925795167u,         203800661629u },
        {   734560801645383190u,   909793965395524173u,         125904685156u },
        { 15648943144911081638u, 12724590949761703756u,         100049320029u },
        { 13664182862003235646u, 10810739657314826395u,          93689801457u },
        {  3895127525902132786u,  2431218615388671301u,         241586051371u },
        {  5249187334214137467u,  4235001167959059286u,          43131796625u },
        { 10642260063359027505u,  6253317787396334247u,         145229579873u },
        {   783598951897779422u,  9534525563070371898u,          97338993036u },
        {  5538576558607624843u,  8392783992374030728u,         140516867666u },
        { 15974581187564609611u, 16356257019231647540u,          82454973731u },
        {  7474269406918257428u, 12896334001521091877u,          35886674469u },
        {  8045286838779138019u,  1427636373320877084u,          37699111667u },
        {  8184246376556341732u, 16116755731295043521u,         243077392322u },
        {  1493267152679331840u, 15945633911163986837u,         194873691078u },
        { 10179074811222818816u,  7510154241072743838u,         198864414546u },
        {  3892499202005008384u,  3571560509790395119u,          82407126277u },
        { 10341173215925108736u,  3576991649007035076u,           5193614683u },
        {  6230307872002015232u, 15509961892750732443u,          91193909105u },
        {  9295429630892703744u, 17789791359353349378u,         113840796718u },
        {                    0u, 18331227331079738314u,          46964386521u },
        {                    0u, 15386712883100476416u,         217993737824u },
        {                    0u, 14082462055028752384u,          96834115376u },
        {                    0u, 12919043128765186048u,          48763411797u },
        {                    0u,  6125373368465096704u,          85700342731u },
        {                    0u, 12335992698065387520u,         203332057155u },
        {                    0u,  2774217370460225536u,          67668735504u },
        {                    0u,                    0u,          16150390625u },
        {                    0u,                    0u,          97000000000u },
        { 10665454627995623288u,                  484u,                    0u },
        { 16803537892767562832u,         484578175453u,                    0u },
        {  8099123106849104444u,  4962829537462579598u,                   26u },
        {  7077413686679401728u,  5711259460785241095u,          26269035528u },
        { 13536636358372449666u, 13845894607204897444u,           8309607995u },
        {  7280632235418610318u, 12116633056637003327u,          59750587450u },
        {  6187823673116858809u,  2965791047992089560u,          58656843994u },
        {  8624343686231740255u, 16021997451315962529u,         218160775854u },
        {   806211610822132771u,  3942052271663803856u,         174868554222u },
        { 18388078233202190882u, 15669876414782439922u,         238213699081u },
        {  7628864426595573600u, 10594415915406145286u,           9849465702u },
        {  4530799784343874981u, 10789820553031921377u,         102574324437u },
        {  8561580552078486438u,  3989990218583987244u,         213584917344u },
        { 13349114951221999594u,  2937341169808224563u,          96216297803u },
        { 10029144738508991772u, 16267436558584536843u,          75159233583u },
        { 12601907197916268979u, 16221580362814625793u,          47881859502u },
        {  1329580921391066941u,  9695437602320209830u,         174879373633u },
        {  3198179786356761112u, 10729753156793715126u,          65525590725u },
        { 11406753413634654142u,  2609241432056861929u,         197581661084u },
        { 11131812960525182090u,  8462663743997037565u,         156141447261u },
        { 14299636753645227208u, 14993422143908194069u,          93458761920u },
        { 12964114684643663326u,  1307443894537745373u,         192812795043u },
        {  5019257593846306316u, 10017257439419829265u,         163070876675u },
        {  6929086798159998121u, 16754772009970777891u,           3543036613u },
        { 18359765766933703649u, 11722573031602862387u,         197908278010u },
        {  9993076234752063198u,  7363764277467092384u,         250635481957u },
        {  3257189215046584509u,  6733958494847390772u,         101399190461u },
        {  2905234736672690348u,  8799796600227451045u,         189365048621u },
        { 12064985302079670056u, 10512023194742249826u,          45477037929u },
        { 14315096064942799930u,  4572542132337197059u,         105569857919u },
        { 11943971043551974038u, 12600500455757416832u,         127247878005u },
        { 12472773152119929647u,  7873789864743195199u,         117683074498u },
        {  7791259796982183085u, 15724851676325671539u,         194426839003u },
        { 16866829442051086686u,  8748017220462413167u,         219852445917u },
        {  3702498393844653053u, 14172589522760466448u,         221474230963u },
        {  2057239613841701716u,  9520545591489413768u,         179768297617u },
        { 17201969976738286226u, 12488551088392570222u,         145516109810u },
        { 18271566505443461640u,  1135798823651241684u,         242677005711u },
        { 18368509115417119804u, 11168725610120161972u,         143061571777u },
        {  7613923684154518587u,  9580104948718508826u,         193605457828u },
        { 17984805084714865232u, 16638722716909738765u,         164519338529u },
        {  4220802739051410373u, 15732724012348272797u,          33901986965u },
        { 17812459042810815760u, 12269722190021214142u,         149852872677u },
        { 10629526089664605307u, 13110655916311972587u,         229665142972u },
        { 17437443267816548473u,  6618112997062866867u,         188710730081u },
        { 12506371893701049304u,  8457936459015989309u,          97358768624u },
        { 15737221540003030739u,  3329167139937134914u,         240458505654u },
        {  2358380859011605513u,  5245511557216705097u,         182180474512u },
        { 10042972713837039706u,  5655931353280440466u,         144284359751u },
        { 13424397272769642495u,   604622132328697393u,          71306608653u },
        {  6867102315755663029u,  8673282619234652338u,          13032776631u },
        {  8690748404825506734u, 16929477433058445690u,         183470179592u },
        {   956229930815387710u, 11036952409253549455u,           8917748810u },
        {  7734449506297687888u, 18199392190170386320u,          74598314388u },
        { 16332184961683848151u,  9683116091880335715u,         148986591027u },
        { 15580777817612768828u,  2993913337608915120u,          51524922775u },
        { 17404245271944696092u,  4490779842162392585u,         151162300367u },
        { 13201420160494469229u,   946849923353644618u,         207243445663u },
        { 16511717657124068078u,  3613491058474899828u,         159051328837u },
        { 14131292492116594062u, 14624054199004410935u,          69195887742u },
        { 18429136031865875691u, 12088470271991908244u,         126792771566u },
        {  2563978348305862197u, 10071980927725011290u,         238655317286u },
        { 14239974392147482896u,  2833441711428854664u,          38546003180u },
        { 11164201396098998272u, 17655572411864340446u,         236153601182u },
        {  7116971104932986880u,  4997642792058747802u,         158957110498u },
        { 12437629862867369984u, 11489200787635734848u,         226270922758u },
        { 16501893821638901760u, 12983586226429536913u,           6622830822u },
        { 10649324268870959104u, 12311150768725063152u,         230703841619u },
        {  7205759403792793600u,  8530052476845967905u,          83667388820u },
        {                    0u,  6282736361499820264u,         148462415071u },
        {                    0u, 11337164765929082880u,         223340587820u },
        {                    0u,  8343856200414134272u,          44614588933u },
        {                    0u, 17889330377156198400u,           5452321350u },
        {                    0u, 17730714064155312128u,          70969782542u },
        {                    0u,  7449235258647511040u,          14961183935u },
        {                    0u,  9943947977234055168u,         191403823852u },
        {                    0u,                    0u,         236539062500u },
        {                    0u,                    0u,         228000000000u },
        {  3016823727048309817u,              7394076u,                    0u },
        { 17405973192644624358u,     7394076163542341u,                    0u },
        {  8937219978302591747u, 12396245121240683569u,               400833u },
        { 15178463196824222317u, 10248996648596888561u,         193672001794u },
        { 11979404627460330594u, 11257495103713935002u,           2555599221u },
        {  2006448052689740002u,  7555396579247433114u,         117610270032u },
        { 16330465320863239865u,  4805022328730367462u,          80409578869u },
        { 17136208883957646553u,  7056637817080232586u,         117260480782u },
        {   140455118208931867u, 10811411483818434913u,          14382541102u },
        {  3964972929179372247u, 16962406704495245447u,          46586087790u },
        {  3378050330022776379u, 18074517319117194669u,         110919533909u },
        {  6065353437512901255u,  3702019776117654523u,          85979821547u },
        { 12768753059854699889u,  3551977551381082764u,         235200686894u },
        {  6848720690951013326u, 16442608985936005282u,          46192553088u },
        { 10953228058585475132u,  3580046275479139588u,         128891355619u },
        { 15539127852083296166u,  8737412692712715330u,         227194074697u },
        { 14534853647735598497u,  3082033243045084752u,          73473656091u },
        {  1506909603576368170u, 16401023756841128699u,          27167077356u },
        {  5475702579938239025u,  7520296082779572869u,         236889101279u },
        { 12039747373985783332u,  9854104766152464159u,         223407676067u },
        { 17572399137760898460u, 14169188802648310188u,         163534192089u },
        {   869552790852091236u,  2018609909210367042u,         217768113264u },
        {  4549585778048181804u,  8270271948267674359u,         112109429062u },
        {  4121918231767210357u, 12320338602894572099u,          70448332340u },
        {  1558397953312543179u, 17538536685990080547u,          52667886893u },
        {  6067524298738069781u, 15833914616956760474u,          45950765978u },
        {  5811095224555517056u,  6137696141415969855u,         154858358231u },
        {  6440894514158997188u,  9757490468419438919u,         215332725174u },
        {  4612748874388784257u,  3566639201356598903u,         182528954618u },
        {  9908101430749813367u,  9760900035773954449u,         250193347898u },
        { 15150017990912190499u,  3873778773990716438u,          58529139451u },
        { 17470426264690059239u,  2295668377270167832u,         251209997968u },
        {  9615161096851907726u,  1791721710912807593u,         144124448432u },
        {  9227932132124142224u, 10571009006922683279u,         176097129428u },
        {  4794113194321211621u,  9840791932778184867u,         212573055546u },
        {   391512698859146347u, 11525464956561274613u,          58533470399u },
        { 17209658878068655842u,  4435781488897895433u,         191624796707u },
        {  3077364726606876150u,  6395563367070996741u,          35240464196u },
        {  1952989567673965814u, 15538690795135662932u,          68346704184u },
        { 11172860676923186449u, 16294558813563371936u,          56842354115u },
        { 17338078544784947239u,  4942096228426070342u,         195883329803u },
        {  4593380466519703278u,  6910116424372647153u,          11267911573u },
        {   786884753602720052u, 17923400669760829478u,         149374598161u },
        { 17848875822468020539u,  4134686917293039955u,          17971629497u },
        {  2899664567187130618u, 16857102463116098681u,         185224141826u },
        {  2973178834961857409u, 11364321508775167451u,           2913825355u },
        { 11117453141176836727u,  7966947780972783899u,          75616061103u },
        {  7535238370146462002u, 11261055695926686278u,         175431889104u },
        { 10246175467290865448u,  9227040437353594058u,         208610463052u },
        {  1203128834127050464u,  7185344074282882061u,          76500198864u },
        { 14518048959078919658u, 14197856148610578032u,         208389518282u },
        {  1553474987376920024u,   885688687260429427u,         202769667324u },
        { 14956572380830948369u, 17407816160380305183u,         252048013279u },
        {  7594162606042048292u, 17812728703806357349u,         223943679604u },
        { 14461296147288811288u, 17120198191964319867u,         116965629957u },
        { 18305427728131488265u, 12091952048375408786u,           5928087803u },
        { 10253208939347909876u,   405056939269888281u,         251655506034u },
        {  6446028915490812012u, 12485440679452408690u,         114021958180u },
        { 12986063676957432257u,  8394369900823444407u,          36676837095u },
        {  9769714697972762807u,  2877421667354294258u,         231455059704u },
        {    97429465146664592u,  2676980714750756239u,         248155985341u },
        { 16395571728207795868u,  6119309228579057021u,         189145119415u },
        {  6331668478323650406u, 18203256146533333852u,         183331728417u },
        { 18129911846294207040u,   351919978865493747u,          33986800493u },
        {  9890094564876124160u,  5190010931882390570u,         109019077620u },
        { 12290856656987750400u,  6982466386088036604u,         244281351056u },
        {  8498454992640802816u,  4707293888784996898u,         144378520261u },
        {  5341660584200896512u,   690306801165964760u,         197255182913u },
        {  9288674231451648000u, 12456770961278956704u,          65037421606u },
        {  1152921504606846976u, 16946092489294063943u,          38675282906u },
        {                    0u, 11098404173866185376u,         218918649514u },
        {                    0u, 15152070965853306880u,         170601645695u },
        {                    0u, 17370091362040414208u,         127821395412u },
        {                    0u, 10141938552171134976u,         212941634539u },
        {                    0u, 10586988556645826560u,         235549795590u },
        {                    0u, 12169852093061922816u,           6573921799u },
        {                    0u, 16717361816799281152u,           7659729003u },
        {                    0u,                    0u,         107906250000u },
        {                    0u,                    0u,          16000000000u },
        { 15212079674427582639u,                  112u,                    0u },
        { 10179808126814248333u,         112824648491u,                    0u },
        {  6066766544199222848u,  2144184049294538808u,                    6u },
        {  6625308131806923532u,  4108002197393276873u,           6116236450u },
        {  9122786786400665713u,  6446230217393892753u,         162222695245u },
        { 11932880778639151320u,  5571068025259989822u,          77349450840u },
        {  3884040911779255011u, 14804812668872528331u,          88302008202u },
        { 13441817515637357872u, 17369928488562523047u,         138802570502u },
        { 11628577856022352826u,  2967474173531035027u,           6941625710u },
        { 13296656925520243654u,  5291425437992807716u,         110160867097u },
        {  5020720704545399398u, 14219547193739388064u,          25286848747u },
        { 14121973606499014694u, 17720313647158217462u,         235770843197u },
        {  3642481034345420114u, 12334850628290578491u,          61960620127u },
        {  9845536238569696768u,  7818499847417334620u,          95668673592u },
        { 17655403572195686356u,   136007040922198372u,          56423841726u },
        {  8377495365136654029u,  8523477092112604449u,         190007372956u },
        { 13713682649609025426u,   367934822655966629u,         156462058619u },
        {  1367709905452854731u, 12964987687054730050u,         123019945786u },
        {  9981467701727208680u, 15267036012420885462u,          58702833390u },
        {  6277920117543306205u, 11142900264750765568u,         238827627680u },
        {  9344111460418701726u, 13680181547777718603u,         160604057833u },
        {  4918507011364617264u, 13001922925761426065u,         233741604127u },
        {  6554777203830755259u,  2397730045956515935u,          31704835654u },
        {  4198262173120265648u,  4482395522588406288u,          70129981206u },
        {  8080325935698446819u,  3255525722490493080u,          22242991148u },
        { 12797633874200091733u,   836222287193822098u,          44176482403u },
        {  8376502502208665497u,   420898743993182306u,          99045331701u },
        {  9891973386793349173u, 11652649973356574054u,         245022816966u },
        { 14877430279003795462u, 15058402726661910231u,         198631691420u },
        {  8759933935842067041u,  9600134495208339559u,         156816317647u },
        {  3753418510388703513u, 14626343323989004842u,         207520424333u },
        {  2961383332545305985u,  6813981265331086665u,         141792895660u },
        {  2497674184068629507u, 10281745288790487888u,         172369386664u },
        { 15244544070742305452u, 17569829347075761940u,         168557374528u },
        {    45953573565810823u,  7654580675237889478u,          64952462357u },
        {  3960077421351906445u, 16194838649686212364u,          21414955649u },
        {  3173330011013883118u,  6495102772252453635u,         129877923962u },
        { 11026266219545759160u, 14935159852819761348u,         122352100226u },
        {  8886757764964685632u, 17381879863441579697u,         130809636637u },
        { 13952322129918090479u,  9062335510435372583u,          29942273595u },
        {  4127250666614902202u,  7569219009130126626u,          59491270192u },
        { 17649958504065306911u, 12652124168176193362u,          48410328184u },
        { 13269305359002216873u,  8940200224697247767u,         120685873025u },
        { 16236593433831947843u,  5600570701927432884u,         129484649225u },
        { 13580930396682424057u,  2018432801986093157u,           9303607546u },
        {   421976357197961116u,  8235849749361824736u,         250109419461u },
        {  2962130818798626533u,  9705097287982370040u,         197446466309u },
        {  8234383947306356345u,  3517483139049842585u,           5526114378u },
        {  4380469931801381425u,   958281614186777760u,          74190683143u },
        { 15279887469027055916u,  7336473432636108950u,           7051948550u },
        {  8534542821713755552u, 12955383920176764423u,           6397711021u },
        {  7814613482565088782u, 10735469126281273789u,         173702312769u },
        { 13771954404705323224u,  8637888232514730693u,          65581970947u },
        {  4568173274762548144u,  6806336737533581000u,           3468260859u },
        {  8105292616250821343u, 16142569672872330321u,         251368972253u },
        {  2923678426777275612u,  8141285259947963513u,         221875090455u },
        {  1808633176918384049u,  5220241098754220797u,          23441339958u },
        { 18118642609460438969u,   154438799943119608u,          54282989837u },
        { 18138164175864360870u,  2226876628677628879u,          13008372144u },
        {  4031433690465792404u, 17219557081221357482u,         176120719223u },
        {  5012226396942308537u, 15401507148161015114u,         119933474059u },
        {  7889712298793536835u,  8842629766613985337u,          11834917375u },
        { 13500762396543628804u,  3180100571546071440u,         255479359920u },
        {   501020624068841347u,  7740848704392475044u,         176172393597u },
        {  8370653768288261750u,  2014314126623495998u,         125419632249u },
        {   647579990023635200u, 11209566016506885858u,         121109196187u },
        { 11106569307181154304u,  7117166613733441125u,         155607671791u },
        { 10797461613892861952u,  4197646860931880328u,         239385822375u },
        { 17627230675448889344u,  5487263271238026094u,         167227554892u },
        { 12197735707942322176u, 18148076225293562697u,          76297465137u },
        { 12871287735024877568u,  9127276943027950849u,          49983809183u },
        {  4611686018427387904u,  9691696125379324722u,         159494790674u },
        {                    0u, 13102362262487705216u,          18525387899u },
        {                    0u,  8929385439893192704u,         123710280481u },
        {                    0u, 11891353410743566336u,          33484062954u },
        {                    0u,  1587423090877399040u,         234644631560u },
        {                    0u,  3489137423026225152u,           8086054378u },
        {                    0u, 13046928120492326912u,         234189146518u },
        {                    0u, 11529215046068469760u,         150707275390u },
        {                    0u,                    0u,         126625000000u },
        {                    0u,                    0u,          64000000000u },
        {  9451803574512021605u,              1721567u,                    0u },
        {  8662766454758138424u,     1721567512383298u,                    0u },
        {  9207992007314947035u,  6674960280855494694u,                93326u },
        { 17118602861291201802u, 16378845781483497510u,         142361850321u },
        { 11186560605745599967u, 17606907750956804392u,         209887899008u },
        { 10505549821532796847u, 13225609159240506969u,         128954472381u },
        { 18133667530488679216u,  2668084873338435252u,         189716961709u },
        { 10778135771244330799u, 14802814305275861366u,         173144637170u },
        { 17087985777033767391u,  8005510553372365574u,         242802462171u },
        {  2639185991757283040u, 12748500143273514429u,         219433979596u },
        {  5401720287293896400u, 10393733905569036127u,         204691097577u },
        {  9816495392633895233u,   603389089974790339u,         233563445444u },
        { 10042051500090034990u,  2033494532597735873u,         196032709788u },
        { 11629689537856384738u,  9204796763694620958u,         156110235959u },
        {  2099086731766010483u,  7826260310402107021u,          55498993032u },
        { 13665537898516458594u, 10122690201685169383u,         136424262421u },
        { 16375672064669490764u,  7438455564568110133u,          21548752135u },
        {  7770550512184564348u,  2805412574380520817u,           7403239484u },
        {  6197048880720627314u,  7250965427231182867u,          60152081720u },
        { 17963594118523106281u,  8136242944826085924u,          56393075623u },
        {  8255454642407818663u, 15357191647956011780u,         167441066613u },
        { 16744201957549498409u,  7369614426695395460u,         117832515027u },
        { 17442505417202859722u, 10886957545142526638u,         211399507598u },
        {  6383975767786687150u,  2030047207417538097u,         142590183151u },
        { 15245442964998335795u, 11557093828502314355u,         239110049079u },
        {  2910016124519524433u, 15201062539664128543u,          55626511311u },
        {  3847913871169988463u,  8846936323343880674u,         207824051251u },
        { 17210451512590059177u,  1485291750116245364u,          51479593379u },
        { 17899408909991454145u,  2076024439668322013u,         163080517827u },
        {  5920601932753251608u,  7029497773682748741u,         195112541510u },
        { 16521781895108979744u, 16333533921668749881u,          70381069837u },
        {  3697493405554698771u,  2065057316131928423u,          13885442648u },
        {  2816108280295732065u,  7800502648925570223u,          88111946981u },
        { 14251078772056398988u, 17011619967093802015u,         229422866095u },
        {  4543066550096031417u,  5368819344429198672u,         175922201766u },
        {  3099369389734296977u, 15598879366754275267u,         166291044279u },
        {  5243484113636490986u, 16393893486035835647u,         183845616944u },
        {  6823370511605197226u, 12042046205096920307u,          48888714746u },
        { 11396290277624641942u, 15437070428008474344u,         250652800632u },
        { 15934361408437566099u, 13704569163204647509u,         120836845264u },
        { 14939272230935131954u, 18192483750856993350u,         208742926182u },
        {  7500666177940329347u,  5152535865317963250u,         102986216520u },
        {  4806714453065462270u, 17512614083933854680u,          72279319528u },
        { 11557851247268441487u, 14481918350603613536u,         232949360711u },
        { 10655523157206817381u, 16124419709964004915u,          71785066366u },
        { 14769600176490881210u, 18088011566435813579u,         126874106543u },
        { 16688495540925795167u, 15008862380698848893u,         175980553071u },
        {   909793965395524173u, 18160498644611827812u,         111813632059u },
        { 12724590949761703756u,  3604680497457231965u,          59984482604u },
        { 10810739657314826395u,  5957615565551495921u,          44195410121u },
        {  2431218615388671301u, 17528455034961565995u,         201322962986u },
        {  4235001167959059286u,  8503772325120113809u,          42950219451u },
        {  6253317787396334247u,  8501492578048509537u,         187460990421u },
        {  9534525563070371898u,  2296237701094386060u,         213460866836u },
        {  8392783992374030728u,  3753593040591076946u,          20124479295u },
        { 16356257019231647540u,  8518075399775653155u,          63203482686u },
        { 12896334001521091877u, 12757855675959554597u,          62461765792u },
        {  1427636373320877084u,   121631169379748595u,         160691604742u },
        { 16116755731295043521u, 16679062494579173314u,           6006593638u },
        { 15945633911163986837u, 10739912744743898054u,         102904173789u },
        {  7510154241072743838u,  9367340677776287570u,         221582211836u },
        {  3571560509790395119u, 12227321512794715397u,         252507804555u },
        {  3576991649007035076u,  7241061891859170651u,         139662844427u },
        { 15509961892750732443u, 13148571323079237489u,          11392538751u },
        { 17789791359353349378u, 12509763434355012654u,         127712785479u },
        { 18331227331079738314u, 11812768946960181977u,          71678155634u },
        { 15386712883100476416u, 14170358803552564832u,         114640371487u },
        { 14082462055028752384u, 18179989524780635952u,          31768176689u },
        { 12919043128765186048u, 17091718978514754901u,          49985539206u },
        {  6125373368465096704u,  7394768384359232459u,         134926543942u },
        { 12335992698065387520u,  6778628272692852803u,          70400871197u },
        {  2774217370460225536u, 18193335045875234320u,          29367470174u },
        {                    0u,  1378519212560967521u,          94986262669u },
        {                    0u,  4677732610631043584u,         141074729676u },
        {                    0u, 17296098591070486528u,         204253580392u },
        {                    0u,  7343735382392963072u,         104937623383u },
        {                    0u, 14525996728454217728u,          87398104692u },
        {                    0u,  9691359370008330240u,         116787455860u },
        {                    0u,  3044433348102455296u,         116525369644u },
        {                    0u,  9223372036854775808u,          44165039062u },
        {                    0u,                    0u,         214500000000u },
        {  4962829537462579598u,                   26u,                    0u },
        {  5711259460785241095u,          26269035528u,                    0u },
        { 13845894607204897444u,  7822291454600056379u,                    1u },
        { 12116633056637003327u,  8201586317771250746u,           1424047269u },
        {  2965791047992089560u,  3278889188817135834u,         165444608885u },
        { 16021997451315962529u,  1710725240251040430u,         117177748939u },
        {  3942052271663803856u,  1850175733663425006u,         203092738601u },
        { 15669876414782439922u,  9147599666163914249u,          41100298227u },
        { 10594415915406145286u, 10221885933644344166u,         243495892371u },
        { 10789820553031921377u, 14901479793736678101u,         147554129546u },
        {  3989990218583987244u,  5181831442059703136u,         138807810838u },
        {  2937341169808224563u,  6396246577759793483u,          22280907645u },
        { 16267436558584536843u, 14167229556464870447u,         125346741221u },
        { 16221580362814625793u,  2969982933326311854u,         229768007053u },
        {  9695437602320209830u,  7892677766222018881u,         141161003097u },
        { 10729753156793715126u,   798698968922663621u,          89427862919u },
        {  2609241432056861929u, 15926812109043458972u,         135043297557u },
        {  8462663743997037565u,  8663842590352697437u,          21863394214u },
        { 14993422143908194069u, 17093523026636671168u,         166469667847u },
        {  1307443894537745373u,   839764004742743203u,           7926641740u },
        { 10017257439419829265u, 16894643909298232323u,          76045523697u },
        { 16754772009970777891u,  9066702926218949317u,         241915860481u },
        { 11722573031602862387u,  9119392417260546810u,           1491506950u },
        {  7363764277467092384u,  9723021096578315109u,           6494363253u },
        {  6733958494847390772u, 14787464248751217597u,         117527086029u },
        {  8799796600227451045u,  3733434565920249133u,         205801630043u },
        { 10512023194742249826u,  6643788868836820841u,          91202389893u },
        {  4572542132337197059u,  4729646697422664063u,         133360160516u },
        { 12600500455757416832u,  4090144564201555829u,           4256394661u },
        {  7873789864743195199u,  2109480737093400002u,         165221727181u },
        { 15724851676325671539u, 16577155033369419739u,         205114355179u },
        {  8748017220462413167u,   745377248603805917u,         235898649375u },
        { 14172589522760466448u, 11305561465807999667u,          31040406981u },
        {  9520545591489413768u,  2211245518782892177u,         197612875715u },
        { 12488551088392570222u, 14170095199249735666u,         195119871859u },
        {  1135798823651241684u, 17849973668116118927u,         115768162399u },
        { 11168725610120161972u,  9020960204585720001u,          95967649011u },
        {  9580104948718508826u, 10807134002871850916u,         243489027232u },
        { 16638722716909738765u,  3925122626254791201u,         160585855908u },
        { 15732724012348272797u, 17208463291312718997u,         164212781323u },
        { 12269722190021214142u,  5145077219589447653u,          11932872664u },
        { 13110655916311972587u, 17602397765035489468u,         216278915194u },
        {  6618112997062866867u, 16422643262490753377u,         122954227894u },
        {  8457936459015989309u,  2902509461400906224u,         182890273275u },
        {  3329167139937134914u,  3422418805967265206u,         251157345353u },
        {  5245511557216705097u,  4228874576277237392u,          73185529695u },
        {  5655931353280440466u,  2553488530807495751u,          95229247750u },
        {   604622132328697393u, 11546099176912486413u,           6138424890u },
        {  8673282619234652338u, 10460791037534167991u,          58625915290u },
        { 16929477433058445690u,  8127117908566000904u,         154567080618u },
        { 11036952409253549455u, 11541304458088287306u,         170440571944u },
        { 18199392190170386320u,  6249718665174839700u,          40625655368u },
        {  9683116091880335715u, 13102508413386290995u,          72338797927u },
        {  2993913337608915120u,  6274675218640661911u,         103710288404u },
        {  4490779842162392585u,  3404497118599817167u,          20340150825u },
        {   946849923353644618u, 11258566093988562335u,          41184558158u },
        {  3613491058474899828u, 16762592482501635397u,          78610328090u },
        { 14624054199004410935u,  5550125446725071998u,          26908701959u },
        { 12088470271991908244u,  6370033225258510318u,           7300872903u },
        { 10071980927725011290u,  1503521728674735398u,         199345320193u },
        {  2833441711428854664u,  4250415082606384364u,           1081506076u },
        { 17655572411864340446u,  6020091901030562974u,          28230415463u },
        {  4997642792058747802u, 16288222967151527138u,         103326349835u },
        { 11489200787635734848u,  6377016228656203782u,          11882986336u },
        { 12983586226429536913u,  8378856515587563750u,          96345698742u },
        { 12311150768725063152u, 15812881490200838483u,         182454218721u },
        {  8530052476845967905u,  4548570371183413652u,         225857218023u },
        {  6282736361499820264u, 16731431495283420383u,         231246578493u },
        { 11337164765929082880u, 14737727629551135532u,          61907012718u },
        {  8343856200414134272u, 12413722258104293893u,         110798933815u },
        { 17889330377156198400u,   800899742400762438u,          55672949232u },
        { 17730714064155312128u,   603197008376033550u,         240043416862u },
        {  7449235258647511040u,  6380777281587743935u,          30032699375u },
        {  9943947977234055168u, 10001440249018225388u,         239345902629u },
        {                    0u,  5505914461980436708u,          37542179162u },
        {                    0u,  1105464290051876864u,          90298476221u },
        {                    0u,  4500443576769970176u,         189059927339u },
        {                    0u,  2843045143185981440u,          43243969535u },
        {                    0u,   660949699682893824u,         255154121786u },
        {                    0u,   276549164618219520u,          58035830155u },
        {                    0u,  4683743612465315840u,         139014991760u },
        {                    0u,                    0u,         144253906250u },
        {                    0u,                    0u,          74000000000u },
        { 12396245121240683569u,               400833u,                    0u },
        { 10248996648596888561u,      400833672001794u,                    0u },
        { 11257495103713935002u,  4370024159708535157u,                21729u },
        {  7555396579247433114u,  7166684413908503888u,         225236899484u },
        {  4805022328730367462u, 10217286283215687029u,         156388506740u },
        {  7056637817080232586u,  4767369911989629198u,         116553880199u },
        { 10811411483818434913u, 14407999214182082862u,         135258439640u },
        { 16962406704495245447u,  8472271297615317358u,         216781059202u },
        { 18074517319117194669u,  6236024012584764757u,         130459282747u },
        {  3702019776117654523u,  1951826556984620523u,          59338055539u },
        {  3551977551381082764u, 12357130551551830830u,         115105808729u },
        { 16442608985936005282u,  8927758011099278464u,          89669881389u },
        {  3580046275479139588u, 10199854049407140323u,          45483974731u },
        {  8737412692712715330u, 17895455027038549577u,          75552935195u },
        {  3082033243045084752u, 16539200343720527131u,          27970114560u },
        { 16401023756841128699u,  3536976106235802604u,            896591847u },
        {  7520296082779572869u, 16980391644793590751u,         231191739858u },
        {  9854104766152464159u, 10090294316609084067u,         210920508875u },
        { 14169188802648310188u, 17603457857266236889u,         203546995950u },
        {  2018609909210367042u, 11164962743035868272u,         238954285362u },
        {  8270271948267674359u,  1585686890718568774u,          50605253843u },
        { 12320338602894572099u, 10882524700472655412u,         211085960258u },
        { 17538536685990080547u,  2194808754940947757u,          66589942846u },
        { 15833914616956760474u,   274100791137209242u,          62118980821u },
        {  6137696141415969855u, 12203404582981010903u,         213014859033u },
        {  9757490468419438919u,   541940706340938166u,          25661547888u },
        {  3566639201356598903u, 10305434016011833594u,         112029378664u },
        {  9760900035773954449u,  7900783531944543546u,         104558658697u },
        {  3873778773990716438u,  8920818625012419323u,         137428302333u },
        {  2295668377270167832u, 12532363335400447632u,         253483598546u },
        {  1791721710912807593u, 13483507182924762800u,         210679380777u },
        { 10571009006922683279u,   415911049779278804u,          41730942389u },
        {  9840791932778184867u,  3441628281170127418u,         181022546583u },
        { 11525464956561274613u, 17830811568183566527u,         151186571042u },
        {  4435781488897895433u, 17897295813176613411u,          34966610231u },
        {  6395563367070996741u,  2086148701331574596u,          55970214350u },
        { 15538690795135662932u, 13015567826878853432u,         206113090347u },
        { 16294558813563371936u, 12944531121587846595u,          43705575345u },
        {  4942096228426070342u,  3534180912913737995u,         177701724438u },
        {  6910116424372647153u,  3447584022400118677u,          22191588331u },
        { 17923400669760829478u,  6375676813770849297u,         235186893904u },
        {  4134686917293039955u, 11580694081479200185u,          80345626132u },
        { 16857102463116098681u,  1872134358882196482u,          20627790684u },
        { 11364321508775167451u, 17602652840520938059u,          92101488606u },
        {  7966947780972783899u, 10331040597716338351u,         222954241722u },
        { 11261055695926686278u,    73785407041056976u,         186560046833u },
        {  9227040437353594058u, 17166209109167902028u,         241003999914u },
        {  7185344074282882061u,  8762475644006589904u,         170930582060u },
        { 14197856148610578032u,  8839001228645872586u,          44475014756u },
        {   885688687260429427u, 13558262784529110268u,         100479163216u },
        { 17407816160380305183u,  5640853896420358111u,          80734994898u },
        { 17812728703806357349u,  8459930353450835572u,         210305791302u },
        { 17120198191964319867u,  7643830211500171269u,          70458613743u },
        { 12091952048375408786u,  1308629115231236347u,         239414372866u },
        {   405056939269888281u,  8957268500971669618u,           2070940926u },
        { 12485440679452408690u,  7645679094277669412u,         254485574498u },
        {  8394369900823444407u,  3821107497040617191u,          98414473094u },
        {  2877421667354294258u,  8847137191985934072u,         134207142652u },
        {  2676980714750756239u,  3531126524756088253u,         252479604268u },
        {  6119309228579057021u,  8726915034124352183u,          44191422752u },
        { 18203256146533333852u, 17611136727168068641u,          32473087011u },
        {   351919978865493747u, 18017743272784259949u,          35954701634u },
        {  5190010931882390570u, 18113575006829616116u,          66976743819u },
        {  6982466386088036604u, 12805550441678740368u,         139981938868u },
        {  4707293888784996898u,  8061966093393027781u,         180694190280u },
        {   690306801165964760u, 11954593141554100801u,         200437040057u },
        { 12456770961278956704u, 14068656112359197734u,         185648059792u },
        { 16946092489294063943u,   895878255770467290u,         144762663376u },
        { 11098404173866185376u, 10319906489512197802u,         208048565657u },
        { 15152070965853306880u, 14551142616794302079u,         153559443251u },
        { 17370091362040414208u, 15933181735739307476u,          51788819021u },
        { 10141938552171134976u, 11524527334398983147u,          77863739512u },
        { 10586988556645826560u, 11828012606225556742u,         120624745878u },
        { 12169852093061922816u,  3556238869349799431u,         150641197848u },
        { 16717361816799281152u,  7403090230513381483u,          24192784095u },
        {                    0u, 10172292854665622800u,         223401322325u },
        {                    0u, 11240746576366182400u,          85551441100u },
        {                    0u, 17021927826892259328u,         204609362092u },
        {                    0u,  9046328496309141504u,         172922760556u },
        {                    0u,  8038996803112140800u,         108490402450u },
        {                    0u, 17098478935265509376u,         146435794889u },
        {                    0u,  7205759403792793600u,         201926910400u },
        {                    0u,                    0u,         192390625000u },
        {                    0u,                    0u,         232000000000u },
        {  2144184049294538808u,                    6u,                    0u },
        {  4108002197393276873u,           6116236450u,                    0u },
        {  6446230217393892753u,  6116236450222695245u,                    0u },
        {  5571068025259989822u,  6240972538554414168u,            331561842u },
        { 14804812668872528331u,  4356262642990299018u,         114338323799u },
        { 17369928488562523047u,  1335108558830511366u,          87236153471u },
        {  2967474173531035027u, 18435704923261947246u,         127072376379u },
        {  5291425437992807716u,  8395401931972636441u,          59999401566u },
        { 14219547193739388064u, 12482665946362458347u,          94455115650u },
        { 17720313647158217462u, 16101242875289374781u,         130676686676u },
        { 12334850628290578491u,  4708983440241068127u,          84872850125u },
        {  7818499847417334620u, 14856666972541426744u,         205255274503u },
        {   136007040922198372u,  6938795288315789246u,           7805381530u },
        {  8523477092112604449u,  5556307628265073820u,         154376152846u },
        {   367934822655966629u,  1441404248927865979u,          14301208040u },
        { 12964987687054730050u, 16710378912353838906u,         232078138680u },
        { 15267036012420885462u, 18289940136919312110u,          56905871455u },
        { 11142900264750765568u, 10217414145292657824u,          95991499641u },
        { 13680181547777718603u, 12461165826430955753u,         121553887130u },
        { 13001922925761426065u,   662762458988270879u,         154675521153u },
        {  2397730045956515935u, 16488546856395302470u,         129035928424u },
        {  4482395522588406288u,  2612816787977180950u,         104893845916u },
        {  3255525722490493080u, 16446616379327454252u,         156141641081u },
        {   836222287193822098u,  7842178508581740643u,         121891572860u },
        {   420898743993182306u, 14779029861369369333u,         124425125348u },
        { 11652649973356574054u,  2697664446153849542u,         228801172814u },
        { 15058402726661910231u, 12135106444393649308u,          78146240682u },
        {  9600134495208339559u,  9550285041205189839u,         170657845438u },
        { 14626343323989004842u,  8790318168586740109u,         190517721989u },
        {  6813981265331086665u, 14038474217155846828u,         133476524102u },
        { 10281745288790487888u,  4263144264274812072u,          70761027212u },
        { 17569829347075761940u, 11940456333341715520u,         140231105513u },
        {  7654580675237889478u, 15751110736831573013u,         233647293434u },
        { 16194838649686212364u, 18384528705472318081u,         250853869423u },
        {  6495102772252453635u,  2393654818032310394u,         111996627298u },
        { 14935159852819761348u, 12812209822018626434u,          98129760287u },
        { 17381879863441579697u,  3110778569433458461u,          31694551286u },
        {  9062335510435372583u,  2860264756226872891u,         246168635644u },
        {  7569219009130126626u,  2384146980060315184u,         252155055263u },
        { 12652124168176193362u, 14117430062880324728u,         159129244866u },
        {  8940200224697247767u,  3769610173216737153u,         194765307417u },
        {  5600570701927432884u, 17731974340232672009u,          25204350976u },
        {  2018432801986093157u,  1971479303384713466u,            961252255u },
        {  8235849749361824736u,  3449462959779012549u,         159106874107u },
        {  9705097287982370040u, 13743454852043766533u,         251186995761u },
        {  3517483139049842585u,  7417711187131879498u,          49745034180u },
        {   958281614186777760u,  3650992383501007879u,         196402114929u },
        {  7336473432636108950u, 12838770342493958662u,         113197920693u },
        { 12955383920176764423u, 16025068246546338477u,         181695991134u },
        { 10735469126281273789u,  6579965938260177729u,          94868720690u },
        {  8637888232514730693u,  4742939430174291459u,          50356700668u },
        {  6806336737533581000u, 13062256857527449083u,         252257115261u },
        { 16142569672872330321u,  2301174570202439645u,         125708106363u },
        {  8141285259947963513u,  7638687886069412887u,         123124746923u },
        {  5220241098754220797u,   936322449610274358u,         171414094100u },
        {   154438799943119608u, 12926010544311283981u,          20050758141u },
        {  2226876628677628879u, 12647854908989899184u,         253700720435u },
        { 17219557081221357482u,  8862093163358513015u,          51685641588u },
        { 15401507148161015114u,   444784343917630731u,         116480415033u },
        {  8842629766613985337u, 11033952249213387263u,          57024111807u },
        {  3180100571546071440u, 18168634046363183536u,         191598151749u },
        {  7740848704392475044u,  3837904761417065597u,          69984923625u },
        {  2014314126623495998u,   111459007020906105u,         233208053234u },
        { 11209566016506885858u, 16191761957496794523u,         242006042204u },
        {  7117166613733441125u,  9856250800340378607u,          92877757174u },
        {  4197646860931880328u,  9491800102275105959u,         246534308426u },
        {  5487263271238026094u, 10777328578953608268u,          74514551514u },
        { 18148076225293562697u, 17424440628313779505u,         218584240152u },
        {  9127276943027950849u,  3285814872419755679u,          24944580819u },
        {  9691696125379324722u,  2824823424107240978u,         211178124381u },
        { 13102362262487705216u, 12271707680713669755u,          93153133984u },
        {  8929385439893192704u,  6951481875178001185u,         160665250606u },
        { 11891353410743566336u, 10202522487003824362u,          46376840587u },
        {  1587423090877399040u,  4834668463880990728u,         139553079852u },
        {  3489137423026225152u, 10871520987687904746u,          44262087902u },
        { 13046928120492326912u, 12057698794225322390u,         222589346333u },
        { 11529215046068469760u,  7263351819222681214u,          29653649161u },
        {                    0u,  1778055686910650944u,           9393747091u },
        {                    0u, 17108187120491986944u,         147096388591u },
        {                    0u,  3067636961549221888u,         239927436682u },
        {                    0u, 16702141595163557888u,         138166296932u },
        {                    0u,  2432053749942845440u,         100905424910u },
        {                    0u, 17791470327927144448u,          14131841897u },
        {                    0u,  1152921504606846976u,         105964477539u },
        {                    0u,                    0u,          99062500000u },
        {                    0u,                    0u,         160000000000u },
        {  6674960280855494694u,                93326u,                    0u },
        { 16378845781483497510u,       93326361850321u,                    0u },
        { 17606907750956804392u,  4283581425266273664u,                 5059u },
        { 13225609159240506969u,  6725911039793895357u,         195232213414u },
        {  2668084873338435252u,  1188689198788975021u,         166364612368u },
        { 14802814305275861366u, 10825527435847761650u,          16064438970u },
        {  8005510553372365574u,  3917696829526085083u,         186586853018u },
        { 12748500143273514429u, 12646861173976387276u,         154212378770u },
        { 10393733905569036127u, 18398576063183996905u,         146685587717u },
        {   603389089974790339u, 16919251228485834948u,           5997388806u },
        {  2033494532597735873u, 17296019588687185052u,           6917194446u },
        {  9204796763694620958u, 12365301604512770359u,         206937619100u },
        {  7826260310402107021u,  2814271599679204744u,         156670324343u },
        { 10122690201685169383u,  2154994415780170517u,         119152561969u },
        {  7438455564568110133u,  6717373824370072839u,          49116822481u },
        {  2805412574380520817u, 12709155755801344060u,         209364149564u },
        {  7250965427231182867u,   826847911966403896u,          60688964714u },
        {  8136242944826085924u,  2277322703890025383u,         106044823515u },
        { 15357191647956011780u,  2774508958389496437u,         219123453911u },
        {  7369614426695395460u,   245697774950120915u,         215150406432u },
        { 10886957545142526638u,  1268929063431863950u,          32013319303u },
        {  2030047207417538097u,  6735665673159411439u,         135068788782u },
        { 11557093828502314355u, 14734771742997073207u,          46365141167u },
        { 15201062539664128543u, 13683287077957612495u,         175798773576u },
        {  8846936323343880674u, 15370263741354826803u,          72741772478u },
        {  1485291750116245364u,    48035913070297507u,         190833223667u },
        {  2076024439668322013u,  1206547475966802115u,         243002604032u },
        {  7029497773682748741u, 13512340386605768006u,             65407069u },
        { 16333533921668749881u,  2325760467700278797u,          93732505440u },
        {  2065057316131928423u, 10848110652847753816u,          96126079727u },
        {  7800502648925570223u, 15846378960784301285u,         239588077256u },
        { 17011619967093802015u, 14121839924449844911u,         200859033924u },
        {  5368819344429198672u,  5147613424753296550u,          68765546476u },
        { 15598879366754275267u, 16817040482828810167u,         236279052682u },
        { 16393893486035835647u,  5773528746119363888u,         138911653591u },
        { 12042046205096920307u,  8716201595536184826u,         215312983620u },
        { 15437070428008474344u,  5259122109038474872u,          68472506235u },
        { 13704569163204647509u, 14744540084230155984u,         123285097580u },
        { 18192483750856993350u, 10719345477982635878u,         108799303119u },
        {  5152535865317963250u, 13698037261310555208u,         207581096882u },
        { 17512614083933854680u, 16141171632951976936u,         178742572087u },
        { 14481918350603613536u, 10060790174955808839u,          55875014667u },
        { 16124419709964004915u,  4250043307981877118u,          11545396528u },
        { 18088011566435813579u,  7075646198054337199u,          48230395309u },
        { 15008862380698848893u, 18141738384245531503u,         173383571548u },
        { 18160498644611827812u,  8174370508376809531u,          92983465608u },
        {  3604680497457231965u,  3581964982731575596u,         136443133513u },
        {  5957615565551495921u, 14798509948722114761u,          73194178710u },
        { 17528455034961565995u, 14713923334885122090u,         150802228831u },
        {  8503772325120113809u,  5042978054260414139u,          95797643382u },
        {  8501492578048509537u,  2052996319372883413u,         118273380388u },
        {  2296237701094386060u,  8825683007899981588u,          36111293153u },
        {  3753593040591076946u,  9992196755378745151u,         225478441234u },
        {  8518075399775653155u,  9301073417573669950u,          18541678071u },
        { 12757855675959554597u,  5331614769144850592u,         247504212200u },
        {   121631169379748595u, 14354009428310052102u,         232289027415u },
        { 16679062494579173314u,  5581221063029119078u,          87778132410u },
        { 10739912744743898054u,  1529260335339476189u,         186302558600u },
        {  9367340677776287570u, 16483061525949201148u,         136082901368u },
        { 12227321512794715397u, 14431217812333089675u,         120893548555u },
        {  7241061891859170651u,  3452349151135392267u,          11782317885u },
        { 13148571323079237489u,  9075317899834447999u,          61187152222u },
        { 12509763434355012654u,  2764331337978901575u,          94491973969u },
        { 11812768946960181977u,  1942890683708857202u,          81149854702u },
        { 14170358803552564832u,   165089169728028447u,         238105324315u },
        { 18179989524780635952u, 15193620741871233073u,          27008949501u },
        { 17091718978514754901u, 14995000835194145926u,         253823647830u },
        {  7394768384359232459u,  1788823614552255558u,          86812880624u },
        {  6778628272692852803u,  8384901184618498845u,         240096972322u },
        { 18193335045875234320u,   405511217862281310u,          34454546404u },
        {  1378519212560967521u,  3111530463755196557u,         228021982807u },
        {  4677732610631043584u,  7893558450035460812u,          87168676404u },
        { 17296098591070486528u,   156573858237402216u,          52427910661u },
        {  7343735382392963072u, 15915324019419451223u,           5008487885u },
        { 14525996728454217728u, 16293363012778802804u,         205862771443u },
        {  9691359370008330240u, 14342105318291351412u,         243883264978u },
        {  3044433348102455296u,  3788398842525387052u,         210777487087u },
        {  9223372036854775808u, 14118764407048307670u,         239205369512u },
        {                    0u,  2705021334614720768u,         168765379752u },
        {                    0u,  7017988973805568000u,         168146639500u },
        {                    0u, 10956732053634154496u,         140380445944u },
        {                    0u, 14657517938546835456u,         248593965634u },
        {                    0u, 11268868284797157376u,          66794585639u },
        {                    0u, 14600669991935148032u,          39610886573u },
        {                    0u,  4611686018427387904u,         173791503906u },
        {                    0u,                    0u,          34250000000u },
        {                    0u,                    0u,         128000000000u },
        {  8201586317771250746u,           1424047269u,                    0u },
        {  3278889188817135834u,  1424047269444608885u,                    0u },
        {  1710725240251040430u,  3001188830946823627u,             77197757u },
        {  1850175733663425006u,  9732296932705387049u,         189162694772u },
        {  9147599666163914249u, 16337535782679529459u,         116527588873u },
        { 10221885933644344166u,  7969742269895046547u,           9885659589u },
        { 14901479793736678101u,  2923592083903829642u,         197432040594u },
        {  5181831442059703136u,  8144196241160608534u,         146158488244u },
        {  6396246577759793483u, 16431078457793424253u,         180441497762u },
        { 14167229556464870447u,   202362949592775653u,         162890730548u },
        {  2969982933326311854u,  8835125248522947981u,          52010970117u },
        {  7892677766222018881u,  7959873808777345113u,           5478953099u },
        {   798698968922663621u, 14929747122315126151u,         139431505623u },
        { 15926812109043458972u,  4310328817360515349u,         215809343213u },
        {  8663842590352697437u,  7294899422760201126u,         237233663393u },
        { 17093523026636671168u,  2047461597291187207u,         161395457290u },
        {   839764004742743203u, 10942374468813517900u,          10110993115u },
        { 16894643909298232323u, 10364795403063433969u,         219593187308u },
        {  9066702926218949317u, 12330859528790939137u,         236561876684u },
        {  9119392417260546810u,  8973160144879916806u,         204668457234u },
        {  9723021096578315109u,  2895354388547509877u,          18486435986u },
        { 14787464248751217597u, 16766844772497556429u,         146156957475u },
        {  3733434565920249133u,  7442407174620948827u,          35908932476u },
        {  6643788868836820841u,  6683013428676659077u,         124403453701u },
        {  4729646697422664063u, 16713703375071907588u,           5362286883u },
        {  4090144564201555829u,  8791044883080637861u,          35906051675u },
        {  2109480737093400002u,   602844107089214413u,          91476563498u },
        { 16577155033369419739u,  9754832281172880875u,          42032680244u },
        {   745377248603805917u, 10587846778003503903u,          52528810517u },
        { 11305561465807999667u, 17206244172922947013u,          21573968323u },
        {  2211245518782892177u, 11620628420699303875u,         195932752365u },
        { 14170095199249735666u, 17864732368219338611u,         237629955528u },
        { 17849973668116118927u,  4146383014621345887u,         200968449082u },
        {  9020960204585720001u, 11445705075042688243u,          58224775873u },
        { 10807134002871850916u,  7369147888966546592u,         193620472915u },
        {  3925122626254791201u,  9762476865090597796u,          83399482307u },
        { 17208463291312718997u,  5507001428194242827u,         195529224931u },
        {  5145077219589447653u, 11371471148365328344u,         227298535145u },
        { 17602397765035489468u,  3148788104946538618u,         233616448686u },
        { 16422643262490753377u,  3762722308424507574u,         174170696145u },
        {  2902509461400906224u,  1156171244825745915u,         209203977585u },
        {  3422418805967265206u, 14208921674868257865u,         113062676168u },
        {  4228874576277237392u,  7903080886897905503u,         200770267187u },
        {  2553488530807495751u,  6367240794154270982u,          51428426873u },
        { 11546099176912486413u,  1623672396662369850u,         121345168815u },
        { 10460791037534167991u, 18323231215381674394u,         175088019456u },
        {  8127117908566000904u,  9842279843006544554u,            993304354u },
        { 11541304458088287306u,  7376839231308610600u,          34533551059u },
        {  6249718665174839700u,   609751749293657672u,         211399899256u },
        { 13102508413386290995u, 10386457966860989799u,         120033054708u },
        {  6274675218640661911u, 11160336020836149780u,         244563051014u },
        {  3404497118599817167u, 17947559933847409193u,           6605003027u },
        { 11258566093988562335u, 10229787001712704590u,          19972939173u },
        { 16762592482501635397u, 10441677090043619866u,         165554557864u },
        {  5550125446725071998u,  4996681336392922375u,         168566044449u },
        {  6370033225258510318u,   124497102381021895u,          33270870638u },
        {  1503521728674735398u,  8180812057779384577u,         110006749001u },
        {  4250415082606384364u,  5294232873532946716u,          73443482710u },
        {  6020091901030562974u,  2885620189169448039u,          86287000939u },
        { 16288222967151527138u, 16662526875008170507u,         107156429783u },
        {  6377016228656203782u, 15663095032402672480u,         215903277391u },
        {  8378856515587563750u,  1824281504410546614u,          79849098083u },
        { 15812881490200838483u,  9506565509584809953u,          99098894498u },
        {  4548570371183413652u, 16941136942345070055u,         162515351948u },
        { 16731431495283420383u, 15924115693705937725u,         140918380873u },
        { 14737727629551135532u,  9247807690406628462u,          73863248041u },
        { 12413722258104293893u,  7993916633864834871u,         169501324659u },
        {   800899742400762438u,  1018504409177639408u,         115433351089u },
        {   603197008376033550u, 12097800686634130718u,         177055213234u },
        {  6380777281587743935u,  6221488888422637551u,         178655823089u },
        { 10001440249018225388u,  8229322865256080421u,         241337267588u },
        {  5505914461980436708u,  7927745108183101786u,         132446112486u },
        {  1105464290051876864u,  8488683721235326653u,         230429763923u },
        {  4500443576769970176u, 11165516518170922283u,          83460172466u },
        {  2843045143185981440u,  5463648141113596927u,         178605283863u },
        {   660949699682893824u,  3958440403860778042u,          23296184959u },
        {   276549164618219520u,  5091534813990256011u,         127214587484u },
        {  4683743612465315840u,  6100166970623291280u,          92276012655u },
        {                    0u,  1913011027739012426u,         111330690714u },
        {                    0u, 11310957650604221440u,         154103704535u },
        {                    0u, 16303817257009020928u,         215613168242u },
        {                    0u,  9090406322154766336u,         114883831704u },
        {                    0u,  3003279315069566976u,         152492791914u },
        {                    0u, 16582887146675765248u,         106162808097u },
        {                    0u,  9691746398101307392u,          33898960113u },
        {                    0u,                    0u,         241525390625u },
        {                    0u,                    0u,          33000000000u },
    };

    static constexpr inline u32 floorLog2(u64 x) {
        return 63 - core::intrin_countLeadingZeros(x);
    }

    static constexpr inline u64 shiftRight128(u64 low, u64 high, u64 dist) {
        Assert(dist < 64);
        Assert(dist > 0);
        return (high << (64 - dist)) | (low >> dist);
    }

    static constexpr inline u64 mulShift(u64 m, const u64* mul, u32 j) {
        u64 high1;
        u64 low1 = umul128(m, mul[1], &high1);
        u64 high0;
        umul128(m, mul[0], &high0);
        u64 sum = high0 + low1;
        if (sum < high0) {
            ++high1; // overflow into high1
        }
        return shiftRight128(sum, high1, j - 64);
    }

    static constexpr inline u32 pow5Factor(u64 value) {
        constexpr u64 mInvDiv = 14757395258967641293u; // 5 * mInvDiv = 1 (mod 2^64)
        constexpr u64 nDiv5 = 3689348814741910323u;    // #{ n | n = 0 (mod 2^64) } = 2^64 / 5
        u32 count = 0;
        for (;;) {
            Assert(value != 0);
            value *= mInvDiv;
            if (value > nDiv5)
                break;
            ++count;
        }
        return count;
    }

    static constexpr inline bool multipleOfPowerOf5(u64 value, u32 p) {
        return pow5Factor(value) >= p;
    }

    static constexpr inline bool multipleOfPowerOf2(u64 value, u32 p) {
        Assert(value != 0);
        Assert(p < 64);
        // __builtin_ctzll doesn't appear to be faster here.
        return (value & ((1ull << p) - 1)) == 0;
    }

    static constexpr inline bool convertFloatToBinary(u64 mantissa, i32 exponent, u64& m2, i32& e2) {
        // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion
        // was exact (trailingZeros).
        bool trailingZeros;
        if (exponent >= 0) {
            // The length of m * 10^e in bits is:
            //   log2(mantissa * 10^exponent) = log2(mantissa) + exponent log2(10) = log2(mantissa) + exponent + exponent * log2(5)
            //
            // We want to compute the MANTISSA_BITS + 1 top-most bits (+1 for the implicit leading
            // one in IEEE format). We therefore choose a binary output exponent of
            //   log2(mantissa * 10^exponent) - (MANTISSA_BITS + 1).
            //
            // We use floor(log2(5^exponent)) so that we get at least this many bits; better to
            // have an additional bit than to not have enough bits.
            e2 = floorLog2(mantissa) + exponent + log2pow5(exponent) - (MANTISSA_BITS + 1);

            // We now compute [mantissa * 10^exponent / 2^e2] = [mantissa * 5^exponent / 2^(e2-exponent)].
            // To that end, we use the POW5_SPLIT table.
            int j = e2 - exponent - ceilLog2pow5(exponent) + POW5_BITCOUNT;
            Assert(j >= 0);

            Assert(exponent < i32(POW5_TABLE_SIZE));
            m2 = mulShift(mantissa, POW5_SPLIT[exponent], j);
            // We also compute if the result is exact, i.e.,
            //   [mantissa * 10^exponent / 2^e2] == mantissa * 10^exponent / 2^e2.
            // This can only be the case if 2^e2 divides mantissa * 10^exponent, which in turn requires that the
            // largest power of 2 that divides mantissa + exponent is greater than e2. If e2 is less than exponent, then
            // the result must be exact. Otherwise we use the existing multipleOfPowerOf2 function.
            trailingZeros = e2 < exponent || (e2 - exponent < 64 && multipleOfPowerOf2(mantissa, e2 - exponent));
        }
        else {
            e2 = floorLog2(mantissa) + exponent - ceilLog2pow5(-exponent) - (MANTISSA_BITS + 1);
            int j = e2 - exponent + ceilLog2pow5(-exponent) - 1 + POW5_INV_BITCOUNT;

            Assert(-exponent < i32(POW5_INV_TABLE_SIZE));
            m2 = mulShift(mantissa, POW5_INV_SPLIT[-exponent], j);
            trailingZeros = multipleOfPowerOf5(mantissa, u32(-exponent));
        }

        return trailingZeros;
    }

    // This is faster if we don't have a 64x64->128-bit multiplication.
    static constexpr inline u64 mulShiftAll64(u64 m, const u64* mul, i32 j, u64* vp, u64* vm, u32 mmShift) {
        m <<= 1;
        // m is maximum 55 bits
        u64 tmp;
        u64 lo = umul128(m, mul[0], &tmp);
        u64 hi;
        u64 mid = tmp + umul128(m, mul[1], &hi);
        hi += mid < tmp; // overflow into hi

        u64 lo2 = lo + mul[0];
        u64 mid2 = mid + mul[1] + (lo2 < lo);
        u64 hi2 = hi + (mid2 < mid);
        *vp = shiftright128(mid2, hi2, u32(j - 64 - 1));

        if (mmShift == 1) {
            u64 lo3 = lo - mul[0];
            u64 mid3 = mid - mul[1] - (lo3 > lo);
            u64 hi3 = hi - (mid3 > mid);
            *vm = shiftright128(mid3, hi3, u32(j - 64 - 1));
        }
        else {
            u64 lo3 = lo + lo;
            u64 mid3 = mid + mid + (lo3 < lo);
            u64 hi3 = hi + hi + (mid3 < mid);
            u64 lo4 = lo3 - mul[0];
            u64 mid4 = mid3 - mul[1] - (lo4 > lo3);
            u64 hi4 = hi3 - (mid4 > mid3);
            *vm = shiftright128(mid4, hi4, u32(j - 64));
        }

        return shiftright128(mid, hi, u32(j - 64 - 1));
    }

    static constexpr inline u32 mulShiftMod1e9(u64 m, const u64* mul, i32 j) {
        u64 high0;
        u64 low0 = umul128(m, mul[0], &high0);
        u64 high1;
        u64 low1 = umul128(m, mul[1], &high1);
        u64 high2;
        u64 low2 = umul128(m, mul[2], &high2);
        [[maybe_unused]] u64 s0low = low0;
        u64 s0high = low1 + high0;
        u32 c1 = s0high < low1;
        u64 s1low = low2 + high1 + c1;
        u32 c2 = s1low < low2;
        u64 s1high = high2 + c2;

        Assert(j >= 128);
        Assert(j <= 180);

        if (j < 160) { // j: [128, 160)
            u64 r0 = mod1e9(s1high);
            u64 r1 = mod1e9((r0 << 32) | (s1low >> 32));
            u64 r2 = ((r1 << 32) | (s1low & 0xffffffff));
            return mod1e9(r2 >> (j - 128));
        }
        else { // j: [160, 192)
            u64 r0 = mod1e9(s1high);
            u64 r1 = ((r0 << 32) | (s1low >> 32));
            return mod1e9(r1 >> (j - 160));
        }
    }
};

template<typename TFloat>
constexpr core::expected<TFloat, ParseError> cstrToFloatImpl(const char* s, u32 slen) {
    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    using Traits = FloatTraits<TFloat>;
    using UInt = typename Traits::UInt;

    constexpr i32 MAX_MANTISSA_DIGITS = Traits::MAX_MANTISSA_DIGITS;
    constexpr i32 MANTISSA_BITS = Traits::MANTISSA_BITS;
    constexpr i32 EXPONENT_BITS = Traits::EXPONENT_BITS;
    constexpr u32 EXPONENT_BIAS = Traits::EXPONENT_BIAS;
    constexpr i32 MIN_EXPONENT_AFTER_WHICH_ROUND_TO_ZERO = Traits::MIN_EXPONENT_AFTER_WHICH_ROUND_TO_ZERO;
    constexpr i32 MAX_EXPONENT_AFTER_WHICH_ROUND_TO_INF = Traits::MAX_EXPONENT_AFTER_WHICH_ROUND_TO_INF;
    constexpr UInt MANTISSA_BITS_MASK = Traits::MANTISSA_BITS_MASK;

    i32 mantissaDigits = 0;
    i32 exponentDigits = 0;
    UInt mantissa = 0;
    i32 exponent = 0;
    u32 dotIndex = slen;
    u32 eIndex = slen;
    bool isNegative = false;
    bool exponentIsNegative = false;

    // Check sign and skip if present
    u32 i = 0;
    if (s[i] == '-') {
        isNegative = true;
        i++;
    }
    else if (s[i] == '+') {
        i++;
    }

    // Check for special strings (NaN, Inf)
    if (slen - i == 3) {
        if ((s[i + 0] == 'n' || s[i + 0] == 'N') &&
            (s[i + 1] == 'a' || s[i + 1] == 'A') &&
            (s[i + 2] == 'n' || s[i + 2] == 'N')
        ) {
            return core::signalingNaN<TFloat>();
        }
        if ((s[i + 0] == 'i' || s[i + 0] == 'I') &&
            (s[i + 1] == 'n' || s[i + 1] == 'N') &&
            (s[i + 2] == 'f' || s[i + 2] == 'F')
        ) {
            return isNegative ? -core::infinity<TFloat>() : core::infinity<TFloat>();
        }
    }

    // Parse mantissa
    for (; i < slen; i++) {
        char c = s[i];
        if (c == '.') {
            if (dotIndex != slen) {
                return core::unexpected(ParseError::InputHasMultipleDots);
            }
            dotIndex = i;
        }
        else {
            if (!core::isDigit(c)) break;
            if (mantissaDigits >= MAX_MANTISSA_DIGITS) return core::unexpected(ParseError::InputNumberTooLarge);
            mantissa = 10 * mantissa + core::toDigit<UInt>(c);
            if (mantissa != 0) mantissaDigits++;
        }
    }

    // At this point either the end of the string has been reached, or a symbol that is not a part of the mantissa.
    if (i < slen && (s[i] == 'e' || s[i] == 'E')) {
        // exponent notation is used.
        eIndex = i;
        i++;
        if (i < slen && ((s[i] == '-') || (s[i] == '+'))) {
            exponentIsNegative = s[i] == '-';
            i++;
        }

        for (; i < slen; i++) {
            char c = s[i];
            if (!core::isDigit(c)) return core::unexpected(ParseError::InputHasInvalidSymbol);
            if (exponentDigits > 3) return core::unexpected(ParseError::InputNumberTooLarge);
            exponent = 10 * exponent + core::toDigit<i32>(c);
            if (exponent != 0) {
                exponentDigits++;
            }
        }
    }

    if (i < slen) {
        // NOTE: This includes white space after the number!
        return core::unexpected(ParseError::InputHasInvalidSymbol);
    }

    if (mantissa == 0) {
        return isNegative ? -0.0f : 0.0f;
    }

    if (exponentIsNegative) {
        exponent = -exponent;
    }

    exponent -= (dotIndex < eIndex) ? (eIndex - i32(dotIndex) - 1) : 0;

    if (mantissaDigits + exponent <= MIN_EXPONENT_AFTER_WHICH_ROUND_TO_ZERO) {
        UInt ieee = UInt(isNegative) << (EXPONENT_BITS + MANTISSA_BITS);
        return core::bitCast<TFloat>(ieee);
    }
    if (mantissaDigits + exponent >= MAX_EXPONENT_AFTER_WHICH_ROUND_TO_INF) {
        UInt ieee = (UInt(isNegative) << UInt(EXPONENT_BITS + MANTISSA_BITS)) | (MANTISSA_BITS_MASK << UInt(MANTISSA_BITS));
        return core::bitCast<TFloat>(ieee);
    }

    UInt m2; i32 e2;
    bool trailingZeros = Traits::convertFloatToBinary(mantissa, exponent, m2, e2);

    // Compute the final IEEE exponent.
    u32 ieee_e2 = std::max(0, e2 + i32(EXPONENT_BIAS) + i32(Traits::floorLog2(m2)));
    constexpr u32 MAX_IEEE_EXPONENT = ((1u << EXPONENT_BITS) - 2);
    if (ieee_e2 > MAX_IEEE_EXPONENT) {
        // Overflow to infinity
        UInt ieee = (UInt(isNegative) << (EXPONENT_BITS + MANTISSA_BITS)) | ((1ull << EXPONENT_BITS) - 1) << MANTISSA_BITS;
        return core::bitCast<TFloat>(ieee);
    }

    // Shift and round
    i32 shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - i32(EXPONENT_BIAS) - MANTISSA_BITS;
    Assert(shift >= 0);

    trailingZeros &= (m2 & ((UInt(1) << (shift - 1)) - 1)) == 0;
    UInt lastRemovedBit = (m2 >> (shift - 1)) & 1;
    bool roundUp = (lastRemovedBit != 0) && (!trailingZeros || ((m2 >> shift) & 1));

    UInt ieee_m2 = (m2 >> shift) + roundUp;
    Assert(ieee_m2 <= (UInt(1) << (MANTISSA_BITS + 1)));
    ieee_m2 &= (UInt(1) << MANTISSA_BITS) - 1;
    if (ieee_m2 == 0 && roundUp) {
        ieee_e2++;
    }

    UInt ieee = ((UInt(isNegative) << EXPONENT_BITS | UInt(ieee_e2)) << MANTISSA_BITS) | ieee_m2;
    return core::bitCast<TFloat>(ieee);
}

constexpr FloatTraits<f32>::FloatDecimal floatToDecimal(u32 ieeeMantissa, u32 ieeeExponent) {
    using Traits = FloatTraits<f32>;
    using FloatDecimal = Traits::FloatDecimal;

    constexpr u32 FLOAT_BIAS = Traits::EXPONENT_BIAS;
    constexpr i32 MANTISSA_BITS = Traits::MANTISSA_BITS;
    constexpr u32 POW5_INV_BITCOUNT = Traits::POW5_INV_BITCOUNT;
    constexpr u32 POW5_BITCOUNT = Traits::POW5_BITCOUNT;

    i32 e2;
    u32 m2;
    if (ieeeExponent == 0) {
        // We subtract 2 so that the bounds computation has 2 additional bits.
        e2 = 1 - i32(FLOAT_BIAS) - MANTISSA_BITS - 2;
        m2 = ieeeMantissa;
    }
    else {
        e2 = i32(ieeeExponent) - FLOAT_BIAS - MANTISSA_BITS - 2;
        m2 = (1u << MANTISSA_BITS) | ieeeMantissa;
    }
    bool even = (m2 & 1) == 0;
    bool acceptBounds = even;

    // Step 2: Determine the interval of valid decimal representations.
    u32 mv = 4 * m2;
    u32 mp = 4 * m2 + 2;
    u32 mmShift = u32(ieeeMantissa != 0) || u32(ieeeExponent <= 1);
    u32 mm = 4 * m2 - 1 - mmShift;

    // Step 3: Convert to a decimal power base using 64-bit arithmetic.
    u32 vr, vp, vm;
    i32 e10;
    bool vmIsTrailingZeros = false;
    bool vrIsTrailingZeros = false;
    u8 lastRemovedDigit = 0;
    if (e2 >= 0) {
        u32 q = log10pow2(e2);
        e10 = i32(q);

        i32 k = i32(POW5_INV_BITCOUNT) + pow5bits(i32(q)) - 1;
        i32 i = -e2 + i32(q) + k;

        vr = Traits::mulShift(mv, Traits::POW5_INV_SPLIT[q], i);
        vp = Traits::mulShift(mp, Traits::POW5_INV_SPLIT[q], i);
        vm = Traits::mulShift(mm, Traits::POW5_INV_SPLIT[q], i);

        if (q != 0 && (vp - 1) / 10 <= vm / 10) {
            // We need to know one removed digit even if we are not going to loop below. We could use
            // q = X - 1 above, except that would require 33 bits for the result, and we've found that
            // 32-bit arithmetic is faster even on 64-bit machines.
            i32 l = POW5_INV_BITCOUNT + pow5bits(i32(q - 1)) - 1;
            i32 shift = -e2 + i32(q) - 1 + l;
            lastRemovedDigit = u8(Traits::mulShift(mv, Traits::POW5_INV_SPLIT[q - 1], u32(shift)) % 10);
        }

        if (q <= 9) {
            // The largest power of 5 that fits in 24 bits is 5^10, but q <= 9 seems to be safe as well.
            // Only one of mp, mv, and mm can be a multiple of 5, if any.
            if (mv % 5 == 0)
                vrIsTrailingZeros = Traits::multipleOfPowerOf5(mv, q);
            else if (acceptBounds)
                vmIsTrailingZeros = Traits::multipleOfPowerOf5(mm, q);
            else
                vp -= Traits::multipleOfPowerOf5(mp, q);
        }
    }
    else {
        u32 q = log10pow5(-e2);
        e10 = i32(q) + e2;

        i32 i = -e2 - i32(q);
        i32 k = pow5bits(i) - POW5_BITCOUNT;
        i32 j = i32(q) - k;

        vr = Traits::mulShift(mv, Traits::POW5_SPLIT[i], j);
        vp = Traits::mulShift(mp, Traits::POW5_SPLIT[i], j);
        vm = Traits::mulShift(mm, Traits::POW5_SPLIT[i], j);

        if (q != 0 && (vp - 1) / 10 <= vm / 10) {
            j = i32(q) - 1 - (pow5bits(i + 1) - POW5_BITCOUNT);
            lastRemovedDigit = u8(Traits::mulShift(mv, Traits::POW5_SPLIT[i + 1], j) % 10);
        }

        if (q <= 1) {
            // {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
            // mv = 4 * m2, so it always has at least two trailing 0 bits.
            vrIsTrailingZeros = true;
            if (acceptBounds) {
                // mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
                vmIsTrailingZeros = mmShift == 1;
            }
            else {
                // mp = mv + 2, so it always has at least one trailing 0 bit.
                --vp;
            }
        }
        else if (q < 31) { // TODO2: bound checking can be tighter here.
            vrIsTrailingZeros = Traits::multipleOfPowerOf2(mv, q - 1);
        }
    }

    // Step 4: Find the shortest decimal representation in the interval of valid representations.
    i32 removed = 0;
    u32 mantissa;
    if (vmIsTrailingZeros || vrIsTrailingZeros) {
        // General case, which happens rarely (~4.0%).
        while (vp / 10 > vm / 10) {
            vmIsTrailingZeros &= vm % 10 == 0;
            vrIsTrailingZeros &= lastRemovedDigit == 0;
            lastRemovedDigit = u8(vr % 10);
            vr /= 10;
            vp /= 10;
            vm /= 10;
            ++removed;
        }

        if (vmIsTrailingZeros) {
            while (vm % 10 == 0) {
                vrIsTrailingZeros &= lastRemovedDigit == 0;
                lastRemovedDigit = u8(vr % 10);
                vr /= 10; vp /= 10; vm /= 10;
                ++removed;
            }
        }

        if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
            // Round even if the exact number is .....50..0.
            lastRemovedDigit = 4;
        }
        // We need to take vr + 1 if vr is outside bounds or we need to round up.
        mantissa = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
    }
    else {
        // Specialized for the common case (~96.0%). Percentages below are relative to this.
        // Loop iterations below (approximately):
        // 0: 13.6%, 1: 70.7%, 2: 14.1%, 3: 1.39%, 4: 0.14%, 5+: 0.01%
        while (vp / 10 > vm / 10) {
            lastRemovedDigit = u8(vr % 10);
            vr /= 10; vp /= 10; vm /= 10;
            removed++;
        }
        // We need to take vr + 1 if vr is outside bounds or we need to round up.
        mantissa = vr + (vr == vm || lastRemovedDigit >= 5);
    }

    i32 exp = e10 + removed;
    FloatDecimal fd = { mantissa, exp };
    return fd;
}

constexpr FloatTraits<f64>::FloatDecimal floatToDecimal(u64 ieeeMantissa, u32 ieeeExponent) {
    using Traits = FloatTraits<f64>;
    using FloatDecimal = Traits::FloatDecimal;

    constexpr u32 DOUBLE_BIAS = Traits::EXPONENT_BIAS;
    constexpr i32 MANTISSA_BITS = Traits::MANTISSA_BITS;
    constexpr u32 POW5_INV_BITCOUNT = Traits::POW5_INV_BITCOUNT;
    constexpr u32 POW5_BITCOUNT = Traits::POW5_BITCOUNT;

    i32 e2;
    u64 m2;
    if (ieeeExponent == 0) {
        // We subtract 2 so that the bounds computation has 2 additional bits.
        e2 = 1 - i32(DOUBLE_BIAS) - MANTISSA_BITS - 2;
        m2 = ieeeMantissa;
    }
    else {
        e2 = i32(ieeeExponent) - DOUBLE_BIAS - MANTISSA_BITS - 2;
        m2 = (1ull << MANTISSA_BITS) | ieeeMantissa;
    }
    bool even = (m2 & 1) == 0;
    bool acceptBounds = even;

    // Step 2: Determine the interval of valid decimal representations.
    u64 mv = 4 * m2;
    u32 mmShift = u32(ieeeMantissa != 0) || u32(ieeeExponent <= 1);

    // Step 3: Convert to a decimal power base using 128-bit arithmetic.
    u64 vr, vp, vm;
    i32 e10;
    bool vmIsTrailingZeros = false;
    bool vrIsTrailingZeros = false;
    if (e2 >= 0) {
        u32 q = log10pow2(e2) - (e2 > 3);
        e10 = i32(q);
        i32 k = POW5_INV_BITCOUNT + pow5bits(i32(q)) - 1;
        i32 i = -e2 + i32(q) + k;

        vr = Traits::mulShiftAll64(m2, Traits::POW5_INV_SPLIT[q], i, &vp, &vm, mmShift);

        if (q <= 21) {
            // Only one of mp, mv, and mm can be a multiple of 5, if any.
            u32 mvMod5 = u32(mv) - 5 * u32(mv / 5);
            if (mvMod5 == 0) {
                vrIsTrailingZeros = Traits::multipleOfPowerOf5(mv, q);
            }
            else if (acceptBounds) {
                // Same as min(e2 + (~mm & 1), pow5Factor(mm)) >= q
                // <=> e2 + (~mm & 1) >= q && pow5Factor(mm) >= q
                // <=> true && pow5Factor(mm) >= q, since e2 >= q.
                vmIsTrailingZeros = Traits::multipleOfPowerOf5(mv - 1 - mmShift, q);
            }
            else {
                // Same as min(e2 + 1, pow5Factor(mp)) >= q.
                vp -= Traits::multipleOfPowerOf5(mv + 2, q);
            }
        }
    }
    else {
        // This expression is slightly faster than max(0, log10Pow5(-e2) - 1).
        u32 q = log10pow5(-e2) - (-e2 > 1);
        e10 = i32(q) + e2;
        i32 i = -e2 - i32(q);
        i32 k = pow5bits(i) - POW5_BITCOUNT;
        i32 j = i32(q) - k;

        vr = Traits::mulShiftAll64(m2, Traits::POW5_SPLIT[i], j, &vp, &vm, mmShift);

        if (q <= 1) {
            // {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
            // mv = 4 * m2, so it always has at least two trailing 0 bits.
            vrIsTrailingZeros = true;
            if (acceptBounds) {
                // mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
                vmIsTrailingZeros = mmShift == 1;
            }
            else {
                // mp = mv + 2, so it always has at least one trailing 0 bit.
                --vp;
            }
        }
        else if (q < 63) { // TODO2: bound checking can be tighter here.
            // We want to know if the full product has at least q trailing zeros.
            // We need to compute min(p2(mv), p5(mv) - e2) >= q
            // <=> p2(mv) >= q && p5(mv) - e2 >= q
            // <=> p2(mv) >= q (because -e2 >= q)
            vrIsTrailingZeros = Traits::multipleOfPowerOf2(mv, q);
        }
    }

    // Step 4: Find the shortest decimal representation in the interval of valid representations.
    i32 removed = 0;
    u8 lastRemovedDigit = 0;
    u64 mantissa;
    // On average, we remove ~2 digits.
    if (vmIsTrailingZeros || vrIsTrailingZeros) {
        // General case, which happens rarely (~0.7%).
        for (;;) {
            u64 vpDiv10 = vp / 10;
            u64 vmDiv10 = vm / 10;
            if (vpDiv10 <= vmDiv10) {
                break;
            }
            u32 vmMod10 = (u32(vm)) - 10 * u32(vmDiv10);
            u64 vrDiv10 = vr / 10;
            u32 vrMod10 = u32(vr) - 10 * u32(vrDiv10);
            vmIsTrailingZeros &= vmMod10 == 0;
            vrIsTrailingZeros &= lastRemovedDigit == 0;
            lastRemovedDigit = u8(vrMod10);
            vr = vrDiv10;
            vp = vpDiv10;
            vm = vmDiv10;
            ++removed;
        }

        if (vmIsTrailingZeros) {
            for (;;) {
                u64 vmDiv10 = vm / 10;
                u32 vmMod10 = u32(vm) - 10 * u32(vmDiv10);
                if (vmMod10 != 0) {
                    break;
                }
                u64 vpDiv10 = vp / 10;
                u64 vrDiv10 = vr / 10;
                u32 vrMod10 = u32(vr) - 10 * u32(vrDiv10);
                vrIsTrailingZeros &= lastRemovedDigit == 0;
                lastRemovedDigit = u8(vrMod10);
                vr = vrDiv10;
                vp = vpDiv10;
                vm = vmDiv10;
                ++removed;
            }
        }

        if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
            // Round even if the exact number is .....50..0.
            lastRemovedDigit = 4;
        }

        // We need to take vr + 1 if vr is outside bounds or we need to round up.
        mantissa = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
    }
    else {
        // Specialized for the common case (~99.3%). Percentages below are relative to this.
        bool roundUp = false;
        u64 vpDiv100 = vp / 100;
        u64 vmDiv100 = vm / 100;
        if (vpDiv100 > vmDiv100) { // Optimization: remove two digits at a time (~86.2%).
            u64 vrDiv100 = vr / 100;
            u32 vrMod100 = u32(vr) - 100 * u32(vrDiv100);
            roundUp = vrMod100 >= 50;
            vr = vrDiv100;
            vp = vpDiv100;
            vm = vmDiv100;
            removed += 2;
        }

        // Loop iterations below (approximately), without optimization above:
        // 0: 0.03%, 1: 13.8%, 2: 70.6%, 3: 14.0%, 4: 1.40%, 5: 0.14%, 6+: 0.02%
        // Loop iterations below (approximately), with optimization above:
        // 0: 70.6%, 1: 27.8%, 2: 1.40%, 3: 0.14%, 4+: 0.02%
        for (;;) {
            u64 vpDiv10 = vp / 10;
            u64 vmDiv10 = vm / 10;
            if (vpDiv10 <= vmDiv10) {
                break;
            }
            u64 vrDiv10 = vr / 10;
            u32 vrMod10 = u32(vr) - 10 * u32(vrDiv10);
            roundUp = vrMod10 >= 5;
            vr = vrDiv10;
            vp = vpDiv10;
            vm = vmDiv10;
            ++removed;
        }

        // We need to take vr + 1 if vr is outside bounds or we need to round up.
        mantissa = vr + (vr == vm || roundUp);
    }

    i32 exp = e10 + removed;
    FloatDecimal fd = { mantissa, exp };
    return fd;
}

constexpr inline i32 toChars(FloatTraits<f32>::FloatDecimal v, bool sign, char* out, [[maybe_unused]] u32 olen) {
    // Step 5: Print the decimal representation.
    i32 index = 0;
    if (sign) {
        out[index++] = '-';
    }

    u32 mantissa = v.mantissa;
    u32 mantissaLen = decimalLength9(mantissa);

    // Print the decimal digits.
    // The following code is equivalent to:
    // for (u32 i = 0; i < mantissaLen - 1; ++i) {
    //     u32 c = mantissa % 10; mantissa /= 10;
    //     result[index + mantissaLen - i] = (char) ('0' + c);
    // }
    // result[index] = '0' + mantissa % 10;
    u32 i = 0;
    while (mantissa >= 10000) {
        u32 c = mantissa % 10000;
        mantissa /= 10000;
        u32 c0 = (c % 100) << 1;
        u32 c1 = (c / 100) << 1;
        core::memcopy(out + index + mantissaLen - i - 1, DIGIT_TABLE + c0, 2);
        core::memcopy(out + index + mantissaLen - i - 3, DIGIT_TABLE + c1, 2);
        i += 4;
    }

    if (mantissa >= 100) {
        u32 c = (mantissa % 100) << 1;
        mantissa /= 100;
        core::memcopy(out + index + mantissaLen - i - 1, DIGIT_TABLE + c, 2);
        i += 2;
    }
    if (mantissa >= 10) {
        u32 c = mantissa << 1;
        // We can't use memcpy here: the decimal dot goes between these two digits.
        out[index + mantissaLen - i] = DIGIT_TABLE[c + 1];
        out[index] = DIGIT_TABLE[c];
    }
    else {
        out[index] = core::digitToChar(char(mantissa));
    }

    // Print decimal point if needed.
    if (mantissaLen > 1) {
        out[index + 1] = '.';
        index += mantissaLen + 1;
    }
    else {
        index++;
    }

    // Print the exponent.
    out[index++] = 'E';
    i32 exp = v.exponent + i32(mantissaLen) - 1;
    if (exp < 0) {
        out[index++] = '-';
        exp = -exp;
    }

    if (exp >= 10) {
        core::memcopy(out + index, DIGIT_TABLE + 2 * exp, 2);
        index += 2;
    }
    else {
        out[index++] = core::digitToChar(char(exp));
    }

    return index;
}

constexpr inline i32 toChars(FloatTraits<f64>::FloatDecimal v, bool sign, char* out, [[maybe_unused]] u32 olen) {
    // Step 5: Print the decimal representation.
    int index = 0;
    if (sign) {
        out[index++] = '-';
    }

    u64 mantissa = v.mantissa;
    u32 olength = decimalLength17(mantissa);

    u32 i = 0;
    // We prefer 32-bit operations, even on 64-bit platforms.
    // We have at most 17 digits, and u32 can store 9 digits.
    // If output doesn't fit into u32, we cut off 8 digits,
    // so the rest will fit into u32.
    if ((mantissa >> 32) != 0) {
        // Expensive 64-bit division.
        u64 q =  mantissa / 100000000;
        u32 mantissa2 = u32(mantissa) - 100000000 * u32(q);
        mantissa = q;

        u32 c = mantissa2 % 10000;
        mantissa2 /= 10000;
        u32 d = mantissa2 % 10000;
        u32 c0 = (c % 100) << 1;
        u32 c1 = (c / 100) << 1;
        u32 d0 = (d % 100) << 1;
        u32 d1 = (d / 100) << 1;
        core::memcopy(out + index + olength - 1, DIGIT_TABLE + c0, 2);
        core::memcopy(out + index + olength - 3, DIGIT_TABLE + c1, 2);
        core::memcopy(out + index + olength - 5, DIGIT_TABLE + d0, 2);
        core::memcopy(out + index + olength - 7, DIGIT_TABLE + d1, 2);
        i += 8;
    }

    u32 mantissa2 = u32(mantissa);
    while (mantissa2 >= 10000) {
        u32 c = mantissa2 % 10000;
        mantissa2 /= 10000;
        u32 c0 = (c % 100) << 1;
        u32 c1 = (c / 100) << 1;
        core::memcopy(out + index + olength - i - 1, DIGIT_TABLE + c0, 2);
        core::memcopy(out + index + olength - i - 3, DIGIT_TABLE + c1, 2);
        i += 4;
    }

    if (mantissa2 >= 100) {
        u32 c = (mantissa2 % 100) << 1;
        mantissa2 /= 100;
        core::memcopy(out + index + olength - i - 1, DIGIT_TABLE + c, 2);
        i += 2;
    }
    if (mantissa2 >= 10) {
        u32 c = mantissa2 << 1;
        // We can't use memcpy here: the decimal dot goes between these two digits.
        out[index + olength - i] = DIGIT_TABLE[c + 1];
        out[index] = DIGIT_TABLE[c];
    }
    else {
        out[index] = core::digitToChar(char(mantissa2));
    }

    // Print decimal point if needed.
    if (olength > 1) {
        out[index + 1] = '.';
        index += olength + 1;
    }
    else {
        index++;
    }

    // Print the exponent.
    out[index++] = 'E';
    i32 exp = v.exponent + i32(olength) - 1;
    if (exp < 0) {
        out[index++] = '-';
        exp = -exp;
    }

    if (exp >= 100) {
        i32 c = exp % 10;
        core::memcopy(out + index, DIGIT_TABLE + 2 * (exp / 10), 2);
        out[index + 2] = core::digitToChar(char(c));
        index += 3;
    }
    else if (exp >= 10) {
        core::memcopy(out + index, DIGIT_TABLE + 2 * exp, 2);
        index += 2;
    }
    else {
        out[index++] = core::digitToChar(char(exp));
    }

    return index;
}

constexpr u32 float32ToCstr(f32 n, char* out, u32 olen) {
    using Traits = FloatTraits<f32>;
    using FloatDecimal = Traits::FloatDecimal;

    constexpr i32 EXPONENT_BITS = Traits::EXPONENT_BITS;

    // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
    bool ieeeSign; u32 ieeeMantissa; u32 ieeeExponent;
    core::decomposeFloat32(n, ieeeMantissa, ieeeExponent, ieeeSign);

    if (ieeeExponent == ((1u << EXPONENT_BITS) - 1u) || (ieeeExponent == 0 && ieeeMantissa == 0)) {
        return copySpecialStr(ieeeSign, ieeeExponent, ieeeMantissa, out, olen);
    }

    FloatDecimal v = floatToDecimal(ieeeMantissa, ieeeExponent);
    return toChars(v, ieeeSign, out, olen);
}

constexpr u32 float64ToCstr(f64 n, char* out, u32 olen) {
    using Traits = FloatTraits<f64>;
    using FloatDecimal = Traits::FloatDecimal;

    constexpr u32 MANTISSA_BITS = Traits::MANTISSA_BITS;
    constexpr i32 EXPONENT_BITS = Traits::EXPONENT_BITS;
    constexpr i32 DOUBLE_BIAS = Traits::EXPONENT_BIAS;

    auto isSmallInt = [](u64 ieeeMantissa, u32 ieeeExponent, FloatDecimal& v) -> bool {
        u64 m2 = (1ull << MANTISSA_BITS) | ieeeMantissa;
        i32 e2 = i32(ieeeExponent) - DOUBLE_BIAS - MANTISSA_BITS;

        if (e2 > 0) {
            return false;
        }

        if (e2 < -52) {
            return false;
        }

        // Since 2^52 <= m2 < 2^53 and 0 <= -e2 <= 52: 1 <= f = m2 / 2^-e2 < 2^53.
        // Test if the lower -e2 bits of the significand are 0, i.e. whether the fraction is 0.
        u64 mask = (1ull << -e2) - 1;
        u64 fraction = m2 & mask;
        if (fraction != 0) {
            return false;
        }

        // f is an integer in the range [1, 2^53).
        // Note: mantissa might contain trailing (decimal) 0's.
        // Note: since 2^53 < 10^16, there is no need to adjust decimalLength17().
        v.mantissa = m2 >> -e2;
        v.exponent = 0;
        return true;
    };

    bool ieeeSign;
    u64 ieeeMantissa;
    u32 ieeeExponent;
    core::decomposeFloat64(n, ieeeMantissa, ieeeExponent, ieeeSign);

    if (ieeeExponent == ((1u << EXPONENT_BITS) - 1u) || (ieeeExponent == 0 && ieeeMantissa == 0)) {
        return copySpecialStr(ieeeSign, ieeeExponent, ieeeMantissa, out, olen);
    }

    FloatDecimal res = {};
    if (isSmallInt(ieeeMantissa, ieeeExponent, res)) {
        // For small integers in the range [1, 2^53), v.mantissa might contain trailing (decimal) zeros.
        // For scientific notation we need to move these zeros into the exponent.
        for (;;) {
            u64 q = res.mantissa / 10;
            u32 r = u32(res.mantissa) - 10 * u32(q);
            if (r != 0) {
                break;
            }
            res.mantissa = q;
            res.exponent++;
        }
    }
    else {
        res = floatToDecimal(ieeeMantissa, ieeeExponent);
    }

    return toChars(res, ieeeSign, out, olen);
}

constexpr u32 float64ToFixedCstr(f64 n, u32 precision, char* out, u32 olen) {
    using Traits = FloatTraits<f64>;

    constexpr u32 MANTISSA_BITS = Traits::MANTISSA_BITS;
    constexpr i32 EXPONENT_BITS = Traits::EXPONENT_BITS;
    constexpr i32 DOUBLE_BIAS = Traits::EXPONENT_BIAS;

    auto indexForExponent = [](u32 e) ->u32 {
       return (e + 15) / 16;
    };
    auto pow10BitsForIndex = [](u32 idx) -> u32 {
        constexpr u32 POW10_ADDITIONAL_BITS = 120;
        return 16 * idx + POW10_ADDITIONAL_BITS;
    };
    auto lengthForIndex = [](u32 idx) -> u32 {
        // +1 for ceil, +16 for mantissa, +8 to round up when dividing by 9
        return (log10pow2(16 * i32(idx)) + 1 + 16 + 8) / 9;
    };

    // Convert `digits` to a sequence of decimal digits. Append the digits to the result.
    // The caller has to guarantee that:
    //   10^(olength-1) <= digits < 10^olength
    // e.g., by passing `olength` as `decimalLength9(digits)`.
    auto appendNDigits = [](u32 olength, u32 digits, char* oout) {
        u32 i = 0;
        while (digits >= 10000) {
            u32 c = digits % 10000;
            digits /= 10000;
            u32 c0 = (c % 100) << 1;
            u32 c1 = (c / 100) << 1;
            core::memcopy(oout + olength - i - 2, DIGIT_TABLE + c0, 2);
            core::memcopy(oout + olength - i - 4, DIGIT_TABLE + c1, 2);
            i += 4;
        }

        if (digits >= 100) {
            u32 c = (digits % 100) << 1;
            digits /= 100;
            core::memcopy(oout + olength - i - 2, DIGIT_TABLE + c, 2);
            i += 2;
        }
        if (digits >= 10) {
            u32 c = digits << 1;
            core::memcopy(oout + olength - i - 2, DIGIT_TABLE + c, 2);
        }
        else {
            oout[0] = core::digitToChar(char(digits));
        }
    };

    bool ieeeSign;
    u64 ieeeMantissa;
    u32 ieeeExponent;
    core::decomposeFloat64(n, ieeeMantissa, ieeeExponent, ieeeSign);

    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << EXPONENT_BITS) - 1u)) {
        return copySpecialStr(ieeeSign, ieeeMantissa, ieeeExponent, out, olen, false);
    }

    if (ieeeExponent == 0 && ieeeMantissa == 0) {
        int index = 0;
        if (ieeeSign) {
            out[index++] = '-';
        }
        out[index++] = '0';
        if (precision > 0) {
            out[index++] = '.';
            core::memset(out + index, '0', precision);
            index += precision;
        }
        return index;
    }

    i32 e2;
    u64 m2;
    if (ieeeExponent == 0) {
        e2 = 1 - DOUBLE_BIAS - MANTISSA_BITS;
        m2 = ieeeMantissa;
    }
    else {
        e2 = i32(ieeeExponent) - DOUBLE_BIAS - MANTISSA_BITS;
        m2 = u64(1ull << MANTISSA_BITS) | ieeeMantissa;
    }

    i32 index = 0;
    bool nonzero = false;
    if (ieeeSign) {
        out[index++] = '-';
    }

    if (e2 >= -52) {
        u32 idx = e2 < 0 ? 0 : indexForExponent(u32(e2));
        u32 p10bits = pow10BitsForIndex(idx);
        i32 len = i32(lengthForIndex(idx));
        for (i32 i = len - 1; i >= 0; --i) {
            u32 j = p10bits - e2;
            u32 digits = Traits::mulShiftMod1e9(m2 << 8, Traits::POW10_SPLIT[Traits::POW10_OFFSET[idx] + i], i32(j + 8));
            if (nonzero) {
                appendNineDigits(digits, out + index);
                index += 9;
            }
            else if (digits != 0) {
                u32 olength = decimalLength9(digits);
                appendNDigits(olength, digits, out + index);
                index += olength;
                nonzero = true;
            }
        }
    }

    if (!nonzero) {
        out[index++] = '0';
    }
    if (precision > 0) {
        out[index++] = '.';
    }

    if (e2 < 0) {
        i32 idx = -e2 / 16;
        u32 blocks = precision / 9 + 1;
        // 0 = don't round up; 1 = round up unconditionally; 2 = round up if odd.
        int roundUp = 0;
        u32 i = 0;
        if (blocks <= Traits::MIN_BLOCK_2[idx]) {
            i = blocks;
            core::memset(out + index, '0', precision);
            index += precision;
        }
        else if (i < Traits::MIN_BLOCK_2[idx]) {
            i = Traits::MIN_BLOCK_2[idx];
            core::memset(out + index, '0', 9 * i);
            index += 9 * i;
        }

        for (; i < blocks; ++i) {
            i32 j = Traits::ADDITIONAL_BITS_2 + (-e2 - 16 * idx);
            u32 p = Traits::POW10_OFFSET_2[idx] + i - Traits::MIN_BLOCK_2[idx];
            if (p >= Traits::POW10_OFFSET_2[idx + 1]) {
                // If the remaining digits are all 0, then we might as well use memset.
                // No rounding required in this case.
                u32 fill = precision - 9 * i;
                core::memset(out + index, '0', fill);
                index += fill;
                break;
            }

            // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above, which is
            // a slightly faster code path in mulShift_mod1e9. Instead, we can just increase the multipliers.
            u32 digits = Traits::mulShiftMod1e9(m2 << 8, Traits::POW10_SPLIT_2[p], j + 8);
            if (i < blocks - 1) {
                appendNineDigits(digits, out + index);
                index += 9;
            }
            else {
                u32 maximum = precision - 9 * i;
                u32 lastDigit = 0;
                for (u32 k = 0; k < 9 - maximum; ++k) {
                    lastDigit = digits % 10;
                    digits /= 10;
                }

                if (lastDigit != 5) {
                    roundUp = lastDigit > 5;
                }
                else {
                    // Is m * 10^(additionalDigits + 1) / 2^(-e2) integer?
                    i32 requiredTwos = -e2 - i32(precision) - 1;
                    bool trailingZeros = requiredTwos <= 0
                        || (requiredTwos < 60 && Traits::multipleOfPowerOf2(m2, u32(requiredTwos)));
                    roundUp = trailingZeros ? 2 : 1;
                }

                if (maximum > 0) {
                    appendDigitsCutExcess(maximum, digits, out + index);
                    index += maximum;
                }

                break;
            }
        }

        if (roundUp != 0) {
            int roundIndex = index;
            int dotIndex = 0; // '.' can't be located at index 0
            while (true) {
                --roundIndex;
                char c;
                if (roundIndex == -1 || (c = out[roundIndex], c == '-')) {
                    out[roundIndex + 1] = '1';
                    if (dotIndex > 0) {
                        out[dotIndex] = '0';
                        out[dotIndex + 1] = '.';
                    }
                    out[index++] = '0';
                    break;
                }

                if (c == '.') {
                    dotIndex = roundIndex;
                    continue;
                }
                else if (c == '9') {
                    out[roundIndex] = '0';
                    roundUp = 1;
                    continue;
                }
                else {
                    if (roundUp == 2 && c % 2 == 0) {
                        break;
                    }
                    out[roundIndex] = c + 1;
                    break;
                }
            }
        }
    }
    else {
        core::memset(out + index, '0', precision);
        index += precision;
    }

    return index;
}

} // namespace detail

template<typename TFloat>
constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen) {
    return detail::cstrToFloatImpl<TFloat>(s, slen);
}

constexpr u32 floatToCstr(f32 n, char* out, u32 olen) {
    return detail::float32ToCstr(n, out, olen);
}
constexpr u32 floatToCstr(f64 n, char* out, u32 olen) {
    return detail::float64ToCstr(n, out, olen);
}

constexpr u32 floatToFixedCstr(f64 n, u32 precision, char* out, u32 olen) {
    return detail::float64ToFixedCstr(n, precision, out, olen);
}

#pragma endregion

} // namespace core
