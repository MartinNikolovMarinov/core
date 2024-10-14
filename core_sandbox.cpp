#include "tests/t-index.h"

#include <iostream>

// TODO: I need to do a bit more research on pow10f
// TODO: I should refactor the pow10 and pow2 for ints. Don't use same function for floats and ints!

static constexpr f32 pow10f_table_f32[] = {
    1e0, 1e1, 1e2,  1e3, 1e4, 1e5, 1e6, 1e7,
    1e8, 1e9, 1e10, 0,   0,   0,   0,   0,
};

static constexpr f64 pow10f_table_f64[] = {
    1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
    1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
    1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22, 0,
    0,    0,    0,    0,    0,    0,    0,    0,
};

template <typename TFloat>
TFloat pow10f(i32 i) {
    if constexpr (sizeof(TFloat) == 4) {
        return pow10f_table_f32[i];
    }
    else if constexpr (sizeof(TFloat) == 8) {
        return pow10f_table_f64[i];
    }
    else {
        static_assert(core::always_false<TFloat>);
    }
}

// Fast-Path
template <typename T>
constexpr T minExponentFastPath() {
    if constexpr (sizeof(T) == 4) return -10;
    return -22;
}
template <typename T>
constexpr T maxExponentFastPath() {
    if constexpr (sizeof(T) == 4) return 10;
    return 22;
}
template <typename T>
constexpr T maxExponentFastPathDisguised() {
    if constexpr (sizeof(T) == 4) return 17;
    return 37;
}
template <typename T>
constexpr T maxMantissaFastPath() {
    if constexpr (sizeof(T) == 4) return 2 << core::mantisaBits<f32>();
    return 2 << core::mantisaBits<f64>();
}
template <typename T>
constexpr T maxMantissaDigits() {
    if constexpr (sizeof(T) == 4) return 19;
    return 38;
}

// // Slow + Eisel-Lemire
// constexpr i32 F32_MANTISSA_EXPLICIT_BITS = core::mantisaBits<f32>();
// constexpr i32 F32_INFINITE_POWER = 0xff;
// // Eisel-Lemire
// constexpr i32 F32_SMALLEST_POWER_OF_TEN = -65;
// constexpr i32 F32_LARGEST_POWER_OF_TEN = 38;
// constexpr i32 F32_MINIMUM_EXPONENT = -127;
// constexpr i32 F32_MIN_EXPONENT_ROUND_TO_EVEN = -17;
// constexpr i32 F32_MAX_EXPONENT_ROUND_TO_EVEN = 10;

// // Slow + Eisel-Lemire
// constexpr u32 F64_MANTISSA_EXPLICIT_BITS = core::mantisaBits<f64>();
// constexpr u32 F64_INFINITE_POWER = 0x7ff;
//  // Eisel-Lemire
// constexpr u32 F64_SMALLEST_POWER_OF_TEN = -342;
// constexpr u32 F64_LARGEST_POWER_OF_TEN = 308;
// constexpr u32 F64_MINIMUM_EXPONENT = -1023;
// constexpr u32 F64_MIN_EXPONENT_ROUND_TO_EVEN = -4;
// constexpr u32 F64_MAX_EXPONENT_ROUND_TO_EVEN = 23;

enum ParseFloatErr : u8 {
    // state changes
    ReachedDot,
    ReachedEndOfBuffer,

    // parse errors
    EmptyInput,
    ScannedInvalidSymbol,
    Overflow,
    NumberTooLarge,

    InvalidInput // general error for unexpected cases
};

template <typename T>
struct Number {
    T exponent = T(0);
    T mantissa = T(0);
    bool isNegative = false;

    void debug__printNumber() {
        std::cout << "negative = " << isNegative << std::endl;
        std::cout << "mantissa = " << mantissa << std::endl;
        std::cout << "exponent = " << exponent << std::endl;
    }
};

// TODO: Should be constexpr
template <typename TMantissaType>
core::expected<Number<TMantissaType>, ParseFloatErr> parseNumber(const char* s, i32 slen, bool isNegative) {
    using TExponentType = TMantissaType;

    i32 idx = 0;

    auto hasSpace = [&](i32 n) -> bool { return idx + n <= slen; };
    auto unsafeAdvance = [&](i32 n) { idx += n; };
    auto advance = [&](i32 n) -> bool {
        if (hasSpace(n)) {
            unsafeAdvance(n);
            return true;
        }
        return false;
    };
    auto currentIs = [&](char x) -> bool {
        if (hasSpace(1)) return s[idx] == x;
        return false;
    };
    auto scanDigit = [&]() -> core::expected<char, ParseFloatErr> {
        if (!hasSpace(1)) return core::unexpected(ParseFloatErr::ReachedEndOfBuffer);

        char v = s[idx];

        if ('0' <= v && v <= '9') {
            unsafeAdvance(1);
            return v - '0';
        }
        else if (v == '.') {
            return core::unexpected(ParseFloatErr::ReachedDot);
        }

        return core::unexpected(ParseFloatErr::ScannedInvalidSymbol);
    };
    auto tryParseDigits = [&](TMantissaType& x) -> core::expected<ParseFloatErr> {
        // Try to parse 8 digits at a time.
        // mFIXME: optimize to read 8 digits at a time!

        constexpr i32 base = 10;

        while (true) {
            auto res = scanDigit();
            if (res.hasErr()) {
                auto err = res.err();
                if (err == ParseFloatErr::ReachedDot ||
                    err == ParseFloatErr::ReachedEndOfBuffer) {
                    break;
                }
                return core::unexpected(err);
            }

            u8 digit = res.value();

            // x = x * base + digit
            if (!core::safeMul(x, TMantissaType(base), x)) {
                return core::unexpected(ParseFloatErr::Overflow);
            }
            if (!core::safeAdd(x, TMantissaType(digit), x)) {
                return core::unexpected(ParseFloatErr::Overflow);
            }
        }

        return {};
    };

    // parse initial digits before dot
    TMantissaType mantissa = 0;
    if (auto res = tryParseDigits(mantissa); res.hasErr()) {
        return core::unexpected(res.err());
    }

    u32 nTotalDigits = u32(idx);
    TExponentType exponent = 0;

    if (currentIs('.')) {
        advance(1);
        TMantissaType marker = TMantissaType(idx);
        if (auto res = tryParseDigits(mantissa); res.hasErr()) {
            return core::unexpected(res.err());
        }
        u32 nAfterDot = u32(idx) - marker;
        exponent = -nAfterDot;
        nTotalDigits += nAfterDot;
    }

    if (nTotalDigits == 0) {
        return core::unexpected(ParseFloatErr::InvalidInput);
    }

    if (i32(nTotalDigits) >= maxMantissaDigits<TMantissaType>()) {
        return core::unexpected(ParseFloatErr::NumberTooLarge);
    }

    Number<TMantissaType> ret;
    ret.mantissa = mantissa;
    ret.isNegative = isNegative;
    ret.exponent = exponent;
    return ret;
}

template <typename TMantissaType>
constexpr bool isFastPath(const Number<TMantissaType>& n) {
    using TExponentType = TMantissaType;

    return minExponentFastPath<TExponentType>() <= n.exponent &&
        n.exponent <= maxExponentFastPathDisguised<TExponentType>() &&
        n.mantissa <= maxMantissaFastPath<TMantissaType>();
}

// TODO: Should be constexpr
template <typename T>
core::expected<T, ParseFloatErr> parseFloat(const char* s, i32 slen) {
    static_assert(std::is_floating_point_v<T>);
    static_assert(sizeof(T) == 4 || sizeof(T) == 8);

    using MantissaType = std::conditional_t<sizeof(T) == 4, i32, i64>;

    if (slen == 0 || s == nullptr) {
        return core::unexpected(ParseFloatErr::EmptyInput);
    }

    // Check if negative
    bool isNegative = s[0] == '-';
    {
        if (s[0] == '-' || s[0] == '+') {
            s++;
            slen--;
            if (slen == 0) {
                // input is only a + or -
                return core::unexpected(ParseFloatErr::InvalidInput);
            }
        }
    }

    // Check for NAN and infinity
    {
        if (slen == 3) {
            if ((s[0] == 'n' || s[0] == 'N') &&
                (s[1] == 'a' || s[1] == 'A') &&
                (s[2] == 'n' || s[2] == 'N')
            ) {
                return core::signalingNaN<T>();
            }
            if ((s[0] == 'i' || s[0] == 'I') &&
                (s[1] == 'n' || s[1] == 'N') &&
                (s[2] == 'f' || s[2] == 'F')
            ) {
                return isNegative ? -core::infinity<T>() : core::infinity<T>();
            }
        }
    }

    auto parseResult = parseNumber<MantissaType>(s, slen, isNegative);
    if (parseResult.hasErr()) {
        return core::unexpected(parseResult.err());
    }
    auto number = parseResult.value();

    // number.debug__printNumber();

    T value;
    if (isFastPath(number)) {
        if (number.exponent <= maxExponentFastPath<MantissaType>()) {
            // normal fast path
            value = T(number.mantissa);
            if (number.exponent < 0)
                value = value / pow10f<T>(-number.exponent);
            else
                value = value * pow10f<T>(number.exponent);
        }
        else {
            // disguised fast path
            MantissaType shift = number.exponent - maxExponentFastPath<MantissaType>();
            MantissaType m;
            if (!core::safeMul(number.mantissa, MantissaType(core::pow10(i32(shift))), m)) {
                Panic(false, "Not fast path, should fallback to slower algorithm.");
            }

            if (m > maxMantissaFastPath<MantissaType>()) {
                Panic(false, "Not fast path, should fallback to slower algorithm.");
            }
            value = T(m) * pow10f<T>(i32(maxExponentFastPath<MantissaType>()));
        }

        if (number.isNegative) {
            value = -value;
        }
    }
    else {
        Panic(false, "TODO: Only fast conversion supported for now.");
    }

    return value;
}

i32 main() {
    constexpr const char* digits = "-123.8";
    constexpr addr_size digitsSize = core::cstrLen(digits);

    std::cout << "input = " << digits << std::endl;
    f32 f1 = core::Unpack(parseFloat<f32>(digits, i32(digitsSize)));
    f32 f2 = -123.8;
    std::cout << "f1 == f2 = " << (f1 == f2 ? "true" : "false") << std::endl;

    return 0;
}
