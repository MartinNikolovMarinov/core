#include "char_ptr.h"

namespace core
{

ptr_size CptrLen(const char* p) {
    // TODO: Should use x86 specific assembler instructions for fast strlen!
    if (p == nullptr) return 0;
    const char *start = p;
    while(*p) p++;
    return p - start;
}

i32 CptrCmp(const char* a, const char* b) {
    return CptrCmp(reinterpret_cast<const uchar *>(a), reinterpret_cast<const uchar *>(b));
}
i32 CptrCmp(const uchar* a, const uchar* b) {
    // FIXME: Crazy unsafe !!
    // TODO: This can be done some much faster with SIMD or some other specifc x86 instruction.
    if (a == nullptr && b == nullptr) return 0;
    else if (a == nullptr) return 1;
    else if (b == nullptr) return -1;

    while (*a && (*a == *b)) {
        ++a; ++b;
    }
    i32 ret = (*a > *b) - (*b  > *a);
    return ret;
}

void CptrCpy(const char *src, ptr_size n, char *dst) {
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

u64 CptrIndexOf(const char *src, ptr_size slen, const char *val, ptr_size vlen) {
    Assert(val != nullptr);
    if (src == nullptr) return -1;
    for (ptr_size i = 0; i < (slen - vlen); i++) {
        i32 cmpVal = CptrCmp(src + i, val);
        if (cmpVal == 0) return i;
    }
    return -1;
}

u64 CptrIndexOfCh(const char *src, ptr_size srcLen, char val) {
    if (src == nullptr) return -1;
    for (ptr_size i = 0; i < srcLen; i++) {
        if (src[i] == val) return i;
    }
    return -1;
}

} // namespace core
