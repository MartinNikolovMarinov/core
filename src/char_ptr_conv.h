#pragma once

#include <API.h>
#include <types.h>
#include <char_ptr.h>
#include <ints.h>
#include <core_traits.h>

// TODO: Write tests for this file!
// TODO: Non of this is even remotely perfromant!

namespace core {

using namespace coretypes;

template <typename TInt>
constexpr inline TInt char_to_int(char s) { return s - '0'; }

template<typename UInt>
constexpr char digit_to_char(UInt digit) {  return (digit % 10) + '0'; }

template<typename TInt>
constexpr void int_to_cptr(TInt n, char* out, u32 digitCount = 0) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    Assert(out != nullptr);
    if (n < 0) {
        *out++ = '-';
        n = -n;
    }
    i32 dc = (digitCount == 0) ? digit_count(n) : digitCount;
    for (i32 i = dc - 1; i >= 0; i--) {
        i32 curr = (n / TInt(pow_u64(10, i))) % 10;
        *out++ = digit_to_char(curr);
        dc--;
    }
}

template <typename TInt>
constexpr TInt cptr_to_int(const char* s) {
    s = cptr_skip_space(s);
    if (s == nullptr || (s[0] != '-' && !is_digit(s[0]))) return 0;
    TInt res = 0;
    bool neg = s[0] == '-';
    if (neg) s++;
    while (is_digit(*s)) {
        res = res * 10 + char_to_int<TInt>(*s);
        s++;
    }
    return neg ? -res : res;
}

template <typename TFloat>
constexpr TFloat cptr_to_float(const char* s) {
    s = cptr_skip_space(s);
    if (s == nullptr || (s[0] != '-' && !is_digit(s[0]))) return 0;
}

namespace detail {
static const char* hexDigits = "0123456789ABCDEF";
} // namespace detail

// The out argument must have enough space to hold the result!
// You can use somthing like - "char out[sizeof(TInt)];"
template <typename TInt>
void int_to_hex(TInt v, char* out, u64 hexLen = (sizeof(TInt) << 1)) {
    static_assert(1 <= sizeof(v) && sizeof(v) <= 8, "Invalid TInt paramater.");
    for (size_t i = 0, j = (hexLen - 1) * 4; i < hexLen; i++, j-=4) {
        out[i] = detail::hexDigits[(v >> j) & 0x0f];
    }
}

} // namespace core
