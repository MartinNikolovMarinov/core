#pragma once

#include <types.h>
#include <utils.h>
#include <core_math.h>
#include <core_traits.h>

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

constexpr inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
constexpr inline bool is_white_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

template<typename TChar>
constexpr addr_size cptr_len(const TChar* p) {
    static_assert(core::is_char_v<TChar>, "TChar must be a char type.");
    if (p == nullptr) return 0;
    const TChar* start = p;
    while(*p) p++;
    return p - start;
}

template <typename TChar>
constexpr i32 cptr_cmp(const TChar* a, addr_size lena, const TChar* b, addr_size lenb) {
    static_assert(core::is_char_v<TChar>, "TChar must be a char type.");

    if (a == nullptr && b == nullptr) return 0;
    else if (a == nullptr) return -1;
    else if (b == nullptr) return 1;

    addr_size i = 0;
    while(i < lena && i < lenb) {
        if (a[i] != b[i]) return (a[i] > b[i]) - (b[i] > a[i]);
        i++;
    }

    if (lena > lenb) return 1;
    else if (lena < lenb) return -1;
    return 0;
}

constexpr bool cptr_eq(const char* a, const char* b, addr_size len) {
    // static_assert(core::is_char_v<TChar>, "TChar must be a char type.");
    return cptr_cmp(a, len, b, len) == 0;
}

constexpr char* cptr_copy(char *dst, const char *src, addr_size len) {
    if (dst == nullptr || src == nullptr) return dst;
    /**
     * TODO2: [PERFORMANCE]
     *
     * AFAIK, the fastest possible copy for x86 (32 and 64-bit) uses 16-byte wide data transfers,
     * which is the size of one XMM register. This is the method recommended in Intel's
     * optimization manual.
     *
     * Could re-use memcopy if there is no need to use intruction set specific instructions for cptr compare.
    */
    for (addr_size i = 0; i < len; i++) {
        *dst++ = *src++;
    }

    return dst;
}

constexpr addr_off cptr_idx_of(const char *s, addr_size slen, const char *val, addr_size vlen) {
    if (s == nullptr || val == nullptr) return -1;
    if (slen < vlen) return -1;
    if (slen == 0 && vlen == 0) return 0;
    for (addr_off i = 0; i < addr_off(slen); i++, s++) {
        i32 cmpVal = cptr_cmp(s, vlen, val, vlen);
        if (cmpVal == 0) return i;
    }
    return -1;
}

constexpr addr_off cptr_idx_of_char(const char *s, addr_size slen, char val) {
    if (s == nullptr) return -1;
    for (addr_off i = 0; i < addr_off(slen); i++) {
        if (s[i] == val) return i;
    }
    return -1;
}

constexpr const char* cptr_skip_space(const char* s) {
    if (s == nullptr) return s;
    while (is_white_space(*s)) s++;
    return s;
}

} // namespace core
