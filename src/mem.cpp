#include "mem.h"

namespace core {

void memcopy(void* dest, const void* src, ptr_size len) {
    // TODO: slow as shit use SSE! William Chan has a good implementation of a fast memcpy.
    char* ddest = reinterpret_cast<char*>(dest);
    const char* ssrc = reinterpret_cast<const char*>(src);
    ptr_size remain = len % 4;
    for (ptr_size i = 0; i < (len - remain); i+=4) {
        ddest[i] = ssrc[i];
        ddest[i+1] = ssrc[i+1];
        ddest[i+2] = ssrc[i+2];
        ddest[i+3] = ssrc[i+3];
    }

    switch (remain) {
    case 1:
        ddest[len-1] = ssrc[len-1];
        break;
    case 2:
        ddest[len-1] = ssrc[len-1];
        ddest[len-2] = ssrc[len-2];
        break;
    case 3:
        ddest[len-1] = ssrc[len-1];
        ddest[len-2] = ssrc[len-2];
        ddest[len-3] = ssrc[len-3];
        break;
    }
}

void* memset(void* dest, i32 c, ptr_size n) {
    // TODO: most naive and slow implementation of memset.
    uchar* p = reinterpret_cast<uchar*>(dest);
    uchar x = uchar(c);
    while (n--) {
        *p++ = x;
    }
    return dest;
}

i32 memcmp(const void* s1, const void* s2, ptr_size n) {
    // TODO: do faster
    const uchar* p1 = reinterpret_cast<const uchar*>(s1);
    const uchar* p2 = reinterpret_cast<const uchar*>(s2);
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        ++p1;
        ++p2;
    }
    return 0;
}

void swap_bytes(void* a, void* b, ptr_size size) {
    // TODO: Use SIMD probably.
    u8* a_ = reinterpret_cast<u8*>(a);
    u8* b_ = reinterpret_cast<u8*>(b);
    for (ptr_size i = 0; i < size; ++i) {
        u8 tmp = a_[i];
        a_[i] = b_[i];
        b_[i] = tmp;
    }
}

}
