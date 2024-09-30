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

    u8* data;
    addr_size dataCap;
    addr_size offset;

    FloatStream() : data(nullptr), dataCap(0), offset(0) {}
    FloatStream(u8* _data, addr_size _dataCap) : data(_data), dataCap(_dataCap), offset(0) {}

    void reset() { offset = 0; }

    addr_size len() {
        if (offset > dataCap) return 0;
        return dataCap - offset;
    }

    bool hasLenFor(addr_size n) { return offset + n <= dataCap; }

    u8 firstUnsafe() { return data[offset]; }

    core::expected<u8, Error> first() {
        if (hasLenFor(1)) {
            return firstUnsafe();
        }
        return core::unexpected(Error::ReachedEndOfBuffer);
    }

    bool firstIs(u8* s, addr_size slen) {
        auto f = first();
        if (f.hasErr()) {
            u8 v = f.value();
            for (addr_size i = 0; i < slen; i++) {
                if (s[i] == v) {
                    return true;
                }
            }
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

    void skipChars(u8* s, addr_size slen) {
        while (firstIs(s, slen)) {
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
    bool manyDigits;
    bool hex;
};

Number parseNumber(const char* s, addr_size slen, bool negative) {

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

i32 main() {
    parseFloat("123.2", core::cstrLen("123.2"));
    return 0;
}
