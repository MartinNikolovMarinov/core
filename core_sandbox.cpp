#include "tests/t-index.h"

#include <iostream>

namespace {

// Fast-Path
constexpr i32 F32_MIN_EXPONENT_FAST_PATH = -10;
constexpr i32 F32_MAX_EXPONENT_FAST_PATH = 10;
constexpr i32 F32_MAX_EXPONENT_FAST_PATH_DISGUISED = 17;
constexpr u32 F32_MAX_MANTISSA_FAST_PATH = 2 << core::mantisaBits<f32>();
// Slow + Eisel-Lemire
constexpr i32 F32_MANTISSA_EXPLICIT_BITS = core::mantisaBits<f32>();
constexpr i32 F32_INFINITE_POWER = 0xff;
// Eisel-Lemire
constexpr i32 F32_SMALLEST_POWER_OF_TEN = -65;
constexpr i32 F32_LARGEST_POWER_OF_TEN = 38;
constexpr i32 F32_MINIMUM_EXPONENT = -127;
constexpr i32 F32_MIN_EXPONENT_ROUND_TO_EVEN = -17;
constexpr i32 F32_MAX_EXPONENT_ROUND_TO_EVEN = 10;

// Fast-Path
constexpr u32 F64_MIN_EXPONENT_FAST_PATH = -22;
constexpr u32 F64_MAX_EXPONENT_FAST_PATH = 22;
constexpr u32 F64_MAX_EXPONENT_FAST_PATH_DISGUISED = 37;
constexpr u64 F64_MAX_MANTISSA_FAST_PATH = u64(2) << core::mantisaBits<f64>();
// Slow + Eisel-Lemire
constexpr u32 F64_MANTISSA_EXPLICIT_BITS = core::mantisaBits<f64>();
constexpr u32 F64_INFINITE_POWER = 0x7ff;
 // Eisel-Lemire
constexpr u32 F64_SMALLEST_POWER_OF_TEN = -342;
constexpr u32 F64_LARGEST_POWER_OF_TEN = 308;
constexpr u32 F64_MINIMUM_EXPONENT = -1023;
constexpr u32 F64_MIN_EXPONENT_ROUND_TO_EVEN = -4;
constexpr u32 F64_MAX_EXPONENT_ROUND_TO_EVEN = 23;

struct FloatStream {
    enum Error: u8 {
        ReachedEndOfBuffer,
        NotADigit
    };

    core::StrView slice;
    addr_size offset;

    FloatStream() : slice(), offset(0) {}
    FloatStream(core::StrView _slice) : slice(_slice), offset(0) {}

    void reset() { offset = 0; }

    addr_size len() {
        if (offset > slice.len()) return 0;
        return slice.len() - offset;
    }

    bool hasLen(addr_size n) { return offset + n <= slice.len(); }

    u8 firstUnsafe() { return slice[offset]; }

    core::expected<u8, Error> first() {
        if (hasLen(1)) {
            return firstUnsafe();
        }
        return core::unexpected(Error::ReachedEndOfBuffer);
    }

    bool firstIsIn(core::StrView s) {
        if (auto f = first(); f.hasValue()) {
            return s.contains(f.value());
        }
        return false;
    }

    bool firstIsLower() {
        if (auto f = first(); f.hasValue()) {
            return core::isLowerCaseANSI(f.value());
        }
        return false;
    }

    bool firstIsDigit(bool isHex = false) {
        if (auto f = first(); f.hasValue()) {
            return isHex ? core::isHexDigit(f.value()) : core::isDigit(f.value());
        }
        return false;
    }

    void advance(addr_size n) {
        offset += n;
    }

    void skipChars(core::StrView chars) {
        while (firstIsIn(chars)) {
            advance(1);
        }
    }

    core::expected<u8, Error> scanDigit(bool isHex = false) {
        if (auto f = first(); f.hasValue()) {
            u8 v = f.value();

            if ('0' <= v && v <= '9') {
                advance(1);
                return v - '0';
            }
            else if (isHex && 'a' <= v && v <= 'f') {
                advance(1);
                return v - 'a' + 10;
            }
            else if (isHex && 'A' <= v && v <= 'F') {
                advance(1);
                return v - 'A' + 10;
            }

            return Error::NotADigit;
        }
        return Error::ReachedEndOfBuffer;
    }
};

struct Number {
    i64 exponent;
    u64 mantissa;
    bool negative;
    bool hex;
};

void tryParseDigits(FloatStream& floatStream, u64& x, i32 base)  {
    if (base == 10) {
        // Try to parse 8 digits at a time.
        // mFIXME: optimize to read 8 digits at a time!
    }

    while (true) {
        if (auto res = floatStream.scanDigit(base == 16); res.hasValue()) {
            u8 digit = res.value();
            x = x * u64(base) + digit;
        }
        else {
            break;
        }
    }
}

Number parseNumber(core::StrView s, bool negative) {
    bool isHex = s.len() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X');
    if (isHex) s = s.slice(2);

    FloatStream floatStream(s);

    // parse initial digits before dot
    u64 mantissa = 0;
    tryParseDigits(floatStream, mantissa, isHex);
    const addr_size intEnd = floatStream.offset;
    const addr_off nDigits = addr_off(floatStream.offset);

    Number ret;
    ret.negative = negative;
    ret.mantissa = mantissa;
    return ret;
}

} // namespace

enum ParseFloatErr : u8 {
    Ok,
    EmptyInputStr,
    InvalidInput
};

ParseFloatErr parseFloat(const char* s, addr_size slen) {
    if (slen == 0 || s == nullptr) {
        return ParseFloatErr::EmptyInputStr;
    }
    else if (core::isWhiteSpace(*s)) {
        return ParseFloatErr::InvalidInput;
    }

    core::StrView m(s, slen);

    addr_size i = 0;
    bool isNegative = s[i] == '-';
    if (s[i] == '-' or s[i] == '+') {
        i += 1;
    }

    if (slen == i) {
        return ParseFloatErr::InvalidInput;
    }

    return ParseFloatErr::Ok;
}

#include <limits>

i32 main() {
    parseFloat("123.2", core::cstrLen("123.2"));

    // FIXME: Write these as tests

    using core::safeAdd;
    using core::safeSub;
    using core::safeMul;
    using core::safeDiv;

    // add and sub test cases:
    {
        int result;

        // 1. Normal Addition Within Bounds
        Assert(safeAdd(10, 20, result) && result == 30);

        // 2. Addition Causing Overflow (Signed)
        Assert(!safeAdd(std::numeric_limits<int>::max(), 1, result));

        // 3. Addition Causing Underflow (Signed)
        Assert(!safeAdd(std::numeric_limits<int>::min(), -1, result));

        // 4. Addition at Maximum Boundary (Unsigned)
        unsigned int uResult;
        Assert(safeAdd(std::numeric_limits<unsigned int>::max() - 1, 1u, uResult) &&
            uResult == std::numeric_limits<unsigned int>::max());

        // 5. Addition Causing Overflow (Unsigned)
        Assert(!safeAdd(std::numeric_limits<unsigned int>::max(), 1u, uResult));

        // 6. Zero Addition
        Assert(safeAdd(0, 0, result) && result == 0);

        // 7. Negative Addition Within Bounds
        Assert(safeAdd(-10, -20, result) && result == -30);

        // 8. Mixed Positive and Negative Addition Within Bounds
        Assert(safeAdd(50, -30, result) && result == 20);

        // 9. Large Positive and Negative Addition Cancelling Out
        Assert(safeAdd(std::numeric_limits<int>::max(), std::numeric_limits<int>::min(), result) &&
            result == -1);

        // 10. Large Unsigned Addition Within Bounds
        Assert(safeAdd(static_cast<unsigned int>(1000000), static_cast<unsigned int>(1000000), uResult) &&
            uResult == 2000000);

        std::cout << "All safeAdd test cases passed!" << std::endl;

        // Test cases for safeSub
        // 1. Normal Subtraction Within Bounds
        Assert(safeSub(30, 20, result) && result == 10);

        // 2. Subtraction Causing Underflow (Signed)
        Assert(!safeSub(std::numeric_limits<int>::min(), 1, result));

        // 3. Subtraction Causing Overflow (Signed)
        Assert(!safeSub(std::numeric_limits<int>::max(), -1, result));

        // 4. Subtraction at Minimum Boundary (Unsigned)
        Assert(safeSub(static_cast<unsigned int>(1), static_cast<unsigned int>(1), uResult) && uResult == 0);

        // 5. Subtraction Causing Underflow (Unsigned)
        Assert(!safeSub(static_cast<unsigned int>(0), static_cast<unsigned int>(1), uResult));

        // 6. Zero Subtraction
        Assert(safeSub(0, 0, result) && result == 0);

        // 7. Positive Minus Negative Within Bounds
        Assert(safeSub(20, -10, result) && result == 30);

        // 8. Negative Minus Positive Within Bounds
        Assert(safeSub(-10, 20, result) && result == -30);

        // 9. Subtraction That Stays Within Bounds
        Assert(safeSub(std::numeric_limits<int>::max(), 100, result) &&
            result == std::numeric_limits<int>::max() - 100);

        // 10. Unsigned Subtraction Without Underflow
        Assert(safeSub(static_cast<unsigned int>(1000000), static_cast<unsigned int>(500000), uResult) &&
            uResult == 500000);

        std::cout << "All safeSub test cases passed!" << std::endl;
    }

    // mul and div test cases:
    {
        int result;

        // Test cases for safeMul
        // 1. Normal Multiplication Within Bounds
        Assert(safeMul(10, 20, result) && result == 200);

        // 2. Multiplication Causing Overflow (Signed)
        Assert(!safeMul(std::numeric_limits<int>::max(), 2, result));

        // 3. Multiplication Causing Underflow (Signed)
        Assert(!safeMul(std::numeric_limits<int>::min(), 2, result));

        // 4. Multiplication by Zero
        Assert(safeMul(0, 1000, result) && result == 0);
        Assert(safeMul(1000, 0, result) && result == 0);

        // 5. Positive and Negative Multiplication Within Bounds
        Assert(safeMul(50, -2, result) && result == -100);

        // 6. Negative Multiplication Within Bounds
        Assert(safeMul(-10, -20, result) && result == 200);

        // 7. Large Unsigned Multiplication Within Bounds
        unsigned int uResult;
        Assert(safeMul(static_cast<unsigned int>(1000000), static_cast<unsigned int>(1000), uResult) && uResult == 1000000000);

        // 8. Unsigned Multiplication Causing Overflow
        Assert(!safeMul(std::numeric_limits<unsigned int>::max(), 2u, uResult));

        // 9. Small Multiplication with Minimum Signed Value (No Overflow)
        Assert(safeMul(std::numeric_limits<int>::min() / 2, 2, result) &&
            result == std::numeric_limits<int>::min());

        // 10. Mixed Sign Multiplication Causing Overflow
        Assert(!safeMul(std::numeric_limits<int>::min(), -1, result));

        std::cout << "All safeMul test cases passed!" << std::endl;

        // Test cases for safeDiv
        // 1. Normal Division Within Bounds
        Assert(safeDiv(100, 20, result) && result == 5);

        // 2. Division by Zero
        Assert(!safeDiv(100, 0, result));

        // 3. Zero Divided by Any Number
        Assert(safeDiv(0, 100, result) && result == 0);

        // 4. Signed Overflow Division (min / -1)
        Assert(!safeDiv(std::numeric_limits<int>::min(), -1, result));

        // 5. Signed Positive Division Within Bounds
        Assert(safeDiv(50, -2, result) && result == -25);

        // 6. Signed Negative Division Within Bounds
        Assert(safeDiv(-100, -20, result) && result == 5);

        // 7. Unsigned Division by Non-zero
        Assert(safeDiv(static_cast<unsigned int>(1000000), static_cast<unsigned int>(100), uResult) &&
            uResult == 10000);

        // 8. Unsigned Division by a Larger Number (Result is 0)
        Assert(safeDiv(static_cast<unsigned int>(50), static_cast<unsigned int>(100), uResult) &&
            uResult == 0);

        // 9. Unsigned Division with Large Divisor
        Assert(safeDiv(std::numeric_limits<unsigned int>::max(), static_cast<unsigned int>(2), uResult) &&
            uResult == std::numeric_limits<unsigned int>::max() / 2);

        // 10. Division of Maximum Signed by a Positive Number
        Assert(safeDiv(std::numeric_limits<int>::max(), 2, result) &&
            result == std::numeric_limits<int>::max() / 2);

        std::cout << "All safeDiv test cases passed!" << std::endl;
    }

    return 0;
}
