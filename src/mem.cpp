#include "mem.h"

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

namespace core {

void* memcopy(void* dest, const void* src, addr_size len) {
    // TODO2: [PERFORMANCE] William Chan has a good implementation of a fast memcpy.
    char* ddest = reinterpret_cast<char*>(dest);
    const char* ssrc = reinterpret_cast<const char*>(src);
    addr_size remain = len % 4;
    for (addr_size i = 0; i < (len - remain); i+=4) {
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

    return dest;
}

void* memset(void* dest, u8 c, addr_size n) {
    u8* p = reinterpret_cast<u8*>(dest);
    for (addr_size i = 0; i < n; ++i) {
        p[i] = c;
    }
    return dest;
}

i32 memcmp(const void* s1, const void* s2, addr_size n) {
    const u8* p1 = reinterpret_cast<const u8*>(s1);
    const u8* p2 = reinterpret_cast<const u8*>(s2);
    for (addr_size i = 0; i < n; ++i) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

void swap_bytes(void* a, void* b, addr_size size) {
    u8* a_ = reinterpret_cast<u8*>(a);
    u8* b_ = reinterpret_cast<u8*>(b);
    for (addr_size i = 0; i < size; ++i) {
        u8 tmp = a_[i];
        a_[i] = b_[i];
        b_[i] = tmp;
    }
}

}
