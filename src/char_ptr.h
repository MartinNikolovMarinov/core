#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>
#include <core_math.h>
#include <ints.h>

namespace core {

using namespace coretypes;

template<typename UInt>
constexpr char digit_to_char(UInt digit) {  return (digit % 10) + '0'; }

template<typename TInt>
constexpr void int_to_cptr(TInt n, char* out, u32 digitCount = 0) {
    static_assert(sizeof(TInt) <= 8, "Invalid TIint paramater.");
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

template<typename TChar>
constexpr ptr_size cptr_len(const TChar* p) {
    // TODO: Should use x86 specific assembler instructions for fast strlen!
    if (p == nullptr) return 0;
    const TChar* start = p;
    while(*p) p++;
    return p - start;
}

template <typename TChar>
constexpr i32 cptr_cmp(const TChar* a, ptr_size lena, const TChar* b, ptr_size lenb) {
    // TODO: This can be done much faster with SIMD or some other specifc x86 instruction.
    static_assert(sizeof(TChar) == 1, "Invalid TChar parameter.");

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
    return cptr_cmp(a, core::cptr_len(a), b, core::cptr_len(b));
}

template <typename TChar>
constexpr i32 cptr_eq(const TChar* a, ptr_size lena, const TChar* b, ptr_size lenb) {
    return cptr_cmp(a, lena, b, lenb) == 0;
}

constexpr void cptr_cpy(const char *src, ptr_size n, char *dst) {
    /**
     * TODO:
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

} // namespace core
