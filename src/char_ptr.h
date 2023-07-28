#pragma once

#include <types.h>
#include <utils.h>
#include <core_math.h>
#include <core_traits.h>

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

// FIXME: src and dest is flipped in all these functions. Fix it.

namespace core {

using namespace coretypes;

constexpr inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
constexpr inline bool is_white_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

template<typename TChar>
constexpr ptr_size cptr_len(const TChar* p) {
    static_assert(core::is_char_v<TChar>, "TChar must be a char type.");
    if (p == nullptr) return 0;
    const TChar* start = p;
    while(*p) p++;
    return p - start;
}

template <typename TChar>
constexpr i32 cptr_cmp(const TChar* a, ptr_size lena, const TChar* b, ptr_size lenb) {
    static_assert(core::is_char_v<TChar>, "TChar must be a char type.");

    if (a == nullptr && b == nullptr) return 0;
    else if (a == nullptr) return -1;
    else if (b == nullptr) return 1;

    ptr_size i = 0;
    while(i < lena && i < lenb) {
        if (a[i] != b[i]) return (a[i] > b[i]) - (b[i] > a[i]);
        i++;
    }

    if (lena > lenb) return 1;
    else if (lena < lenb) return -1;
    return 0;
}

// Avoid this if you already know the length of the string.
template <typename TChar>
constexpr i32 cptr_cmp(const TChar* a, const TChar* b) {
    static_assert(core::is_char_v<TChar>, "TChar must be a char type.");
    return cptr_cmp(a, core::cptr_len(a), b, core::cptr_len(b));
}

constexpr bool cptr_eq(const char* a, const char* b, ptr_size len) {
    // static_assert(core::is_char_v<TChar>, "TChar must be a char type.");
    return cptr_cmp(a, len, b, len) == 0;
}

constexpr void cptr_copy(const char *src, ptr_size n, char *dst) {
    /**
     * TODO2: [PERFORMANCE]
     * This is slow as shit.
     *
     * AFAIK, the fastest possible copy for x86 (32 and 64-bit) uses 16-byte wide data transfers,
     * which is the size of one XMM register. This is the method recommended in Intel's
     * optimization manual.
    */
    for (ptr_size i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

constexpr ptr_size cptr_idx_of(const char *src, ptr_size slen, const char *val, ptr_size vlen) {
    if (src == nullptr || val == nullptr) return -1;
    if (slen < vlen) return -1;
    if (slen == 0 && vlen == 0) return 0;
    for (ptr_size i = 0; i < slen; i++, src++) {
        i32 cmpVal = cptr_cmp(src, vlen, val, vlen);
        if (cmpVal == 0) return i;
    }
    return -1;
}

constexpr ptr_size cptr_idx_of_char(const char *src, ptr_size slen, char val) {
    if (src == nullptr) return -1;
    for (ptr_size i = 0; i < slen; i++) {
        if (src[i] == val) return i;
    }
    return -1;
}

constexpr const char* cptr_skip_space(const char* s) {
    if (s == nullptr) return s;
    while (is_white_space(*s)) s++;
    return s;
}

} // namespace core
