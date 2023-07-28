#include "mem.h"

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

namespace core {

void memcopy(void* dest, const void* src, ptr_size len) {
    // TODO2: [PERFORMANCE] William Chan has a good implementation of a fast memcpy.
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
    u8* p = reinterpret_cast<u8*>(dest);
    u8 x = u8(c);
    while (n--) {
        *p++ = x;
    }
    return dest;
}

i32 memcmp(const void* s1, const void* s2, ptr_size n) {
    const u8* p1 = reinterpret_cast<const u8*>(s1);
    const u8* p2 = reinterpret_cast<const u8*>(s2);
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        ++p1;
        ++p2;
    }
    return 0;
}

void swap_bytes(void* a, void* b, ptr_size size) {
    u8* a_ = reinterpret_cast<u8*>(a);
    u8* b_ = reinterpret_cast<u8*>(b);
    for (ptr_size i = 0; i < size; ++i) {
        u8 tmp = a_[i];
        a_[i] = b_[i];
        b_[i] = tmp;
    }
}

}
