#pragma once

#include <types.h>
#include <char_ptr.h>
#include <ints.h>
#include <core_traits.h>

// TODO2: [PERFORMACE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

template <typename Ta>
constexpr inline Ta char_to_int(char s) {
    static_assert(core::is_arithmetic_v<Ta>, "TInt must be an arithmetic type.");
    return static_cast<Ta>(s - '0');
}

template<typename TInt>
constexpr char digit_to_char(TInt digit) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    return static_cast<char>((digit % 10) + '0');
}

namespace detail {

template<typename TInt>
constexpr void int_to_cptr(TInt n, char* out, u32 digitCount) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    Assert(out != nullptr);
    if constexpr (core::is_signed_v<TInt>) {
        if (n < 0) {
            *out++ = '-';
            n = -n;
        }
    }
    i32 dc = (digitCount == 0) ? digit_count(n) : digitCount;
    for (i32 i = dc - 1; i >= 0; i--) {
        // There is a lot of believe in all this static casting, but it 'should not' be dangerous.
        i32 curr = static_cast<i32>((n / static_cast<TInt>(pow10(i)))) % 10;
        *out++ = digit_to_char(curr);
        dc--;
    }
}

} // detail namespace

constexpr void int_to_cptr(u32 n, char* out, u32 digitCount = 0) { detail::int_to_cptr(n, out, digitCount); }
constexpr void int_to_cptr(u64 n, char* out, u32 digitCount = 0) { detail::int_to_cptr(n, out, digitCount); }
constexpr void int_to_cptr(i32 n, char* out, u32 digitCount = 0) { detail::int_to_cptr(n, out, digitCount); }
constexpr void int_to_cptr(i64 n, char* out, u32 digitCount = 0) { detail::int_to_cptr(n, out, digitCount); }

template <typename Invalid>
constexpr Invalid cptr_to_int(Invalid, char*, [[maybe_unused]] u32 digitCount = 0) {
    static_assert(core::always_false<Invalid>, "Invalid type passed to cptr_to_int.");
    return 0;
}

// This function does not handle overflows!
template <typename TInt>
constexpr TInt cptr_to_int(const char* s) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    s = cptr_skip_space(s);
    if (s == nullptr || (s[0] != '-' && !is_digit(s[0]))) return 0;
    TInt res = 0;
    bool neg = s[0] == '-';
    if (neg) s++;
    while (is_digit(*s)) {
        res = res * 10 + char_to_int<TInt>(*s);
        s++;
    }
    if constexpr (core::is_signed_v<TInt>) {
        if (neg) res = -res;
    }
    return res;
}

// This function does not handle overflows!
template <typename TFloat>
constexpr TFloat cptr_to_float(const char* s) {
    static_assert(core::is_float_v<TFloat>, "TFloat must be a floating point type.");
    s = cptr_skip_space(s);
    if (s == nullptr || ((s[0] != '-' && s[0] != '.') && !is_digit(s[0]))) return 0;
    TFloat res = 0;
    bool neg = s[0] == '-';
    i32 fractionalPart = 0;
    if (neg) s++;

    while(true) {
        if (*s == '.') {
            if (fractionalPart > 0) break; // Number has more than one decimal point.
            fractionalPart = 1;
            s++;
        }
        if (!is_digit(*s)) {
            break;
        }

        if (fractionalPart > 0) {
            res += char_to_int<TFloat>(*s) / TFloat(pow10(fractionalPart++));
        }
        else {
            res = res * 10 + char_to_int<TFloat>(*s);
        }

        s++;
    };

    return neg ? -res : res;
}

namespace detail {
static constexpr const char* hexDigits = "0123456789ABCDEF";
} // namespace detail

// The out argument must have enough space to hold the result!
// You can use somthing like - "char out[sizeof(TInt)];"
template <typename TInt>
constexpr void int_to_hex(TInt v, char* out, u64 hexLen = (sizeof(TInt) << 1)) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    for (size_t i = 0, j = (hexLen - 1) * 4; i < hexLen; i++, j-=4) {
        out[i] = detail::hexDigits[(v >> j) & 0x0f];
    }
}

} // namespace core
