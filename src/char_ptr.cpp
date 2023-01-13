#include <char_ptr.h>

namespace core {

ptr_size cptr_len(const char* p) {
    // TODO: Should use x86 specific assembler instructions for fast strlen!
    if (p == nullptr) return 0;
    const char *start = p;
    while(*p) p++;
    return p - start;
}

i32 cptr_cmp(const char* a, const char* b) {
    return cptr_cmp(reinterpret_cast<const uchar *>(a), reinterpret_cast<const uchar *>(b));
}
i32 cptr_cmp(const uchar* a, const uchar* b) {
    // FIXME: Should implement a safe version of this !!
    // TODO: This can be done much faster with SIMD or some other specifc x86 instruction.
    if (a == nullptr && b == nullptr) return 0;
    else if (a == nullptr) return 1;
    else if (b == nullptr) return -1;

    while (*a && (*a == *b)) {
        ++a; ++b;
    }
    i32 ret = (*a > *b) - (*b  > *a);
    return ret;
}

void cptr_cpy(const char *src, ptr_size n, char *dst) {
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

u64 cptr_idx_of(const char *src, ptr_size slen, const char *val, ptr_size vlen) {
    Assert(val != nullptr);
    if (src == nullptr) return -1;
    for (ptr_size i = 0; i < (slen - vlen); i++) {
        i32 cmpVal = cptr_cmp(src + i, val);
        if (cmpVal == 0) return i;
    }
    return -1;
}

u64 cptr_idx_of_char(const char *src, ptr_size srcLen, char val) {
    if (src == nullptr) return -1;
    for (ptr_size i = 0; i < srcLen; i++) {
        if (src[i] == val) return i;
    }
    return -1;
}

} // namespace core
