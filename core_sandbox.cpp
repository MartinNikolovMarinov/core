#include "tests/t-index.h"

#include "__original_ryu/ryu_parse.h"

#include <iomanip>
#include <iostream>

// TODO: Don't forget to give credit to the creator of this code and mention the license.

enum struct ParseFloatErr : u8 {
    InputEmpty,
    InputHasMultipleDots,
    InputHasInvalidSymbol,
    InputNumberTooLarge, // will overflow return type
};

const char* parseFloatErrCStr(ParseFloatErr err) {
    switch (err) {
        case ParseFloatErr::InputEmpty:             return "Input is empty";
        case ParseFloatErr::InputHasMultipleDots:   return "Input more than one dot";
        case ParseFloatErr::InputHasInvalidSymbol:  return "Input contains an invalid symbol";
        case ParseFloatErr::InputNumberTooLarge:    return "Input number is too large to fit in return type";
    }
    return "Unknown";
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
constexpr const u64 FLOAT_POW5_SPLIT[47] = {
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

constexpr inline u32 mulPow5divPow2(u32 m, u32 i, u32 j) {
    return mulShift32(m, FLOAT_POW5_SPLIT[i], j);
}

constexpr inline i32 mulPow5InvDivPow2(u32 m, u32 i, u32 j) {
    return mulShift32(m, FLOAT_POW5_INV_SPLIT[i], j);
}

inline f32 int32Bits2Float(u32 bits) {
    // FIXME: This makes the algorithm unusable in constexpressions. Can this be avoided?
    f32 f;
    core::memcopy(reinterpret_cast<u32*>(&f), &bits, 1);
    return f;
}

core::expected<f32, ParseFloatErr> my_s2f(const char* s, const u32 slen) {
    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseFloatErr::InputEmpty);
    }

    constexpr i32 MAX_MANTISA_DIGITS = i32(core::maxMantisaDigitsBase10<f32>());
    constexpr i32 MANTISSA_BITS = i32(core::mantisaBits<f32>());
    constexpr i32 EXPONENT_BITS = i32(core::exponentBits<f32>());

    constexpr u32 FLOAT_POW5_INV_BITCOUNT = 59;
    constexpr u32 FLOAT_EXPONENT_BIAS = 127;

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

    for (; i < slen; i++) {
        char c = s[i];
        if (c == '.') {
            if (dotIndex != slen) {
                return core::unexpected(ParseFloatErr::InputHasMultipleDots);
            }
            dotIndex = i;
        }
        else {
            // TODO: Instead of returning InputHasInvalidSymbol I could just break and do a best effort parse.
            if (!core::isDigit(c)) return core::unexpected(ParseFloatErr::InputHasInvalidSymbol);
            if (mantissaDigits >= MAX_MANTISA_DIGITS) return core::unexpected(ParseFloatErr::InputNumberTooLarge);
            mantissa = 10 * mantissa + core::toDigit(c);
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
        f32 res = int32Bits2Float(ieee);
        return res;
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
    f32 ret = int32Bits2Float(ieee);
    return ret;
}

i32 main() {
    constexpr const char* digits = "01.126";
    constexpr addr_size digitsSize = core::cstrLen(digits);
    // f32 r;
    // auto status = s2f_n(digits, digitsSize, &r);
    // std::cout << r << std::endl;

    auto res = my_s2f(digits, digitsSize);
    if (res.hasErr()) {
        std::cout << parseFloatErrCStr(res.err()) << std::endl;
        return 0;
    }
    std::cout << res.value() << std::endl;
    return 0;
}
