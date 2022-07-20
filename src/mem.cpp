#include "mem.h"

namespace core
{

void MemCopy(void* dest, const void* src, ptr_size len) {
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

void* MemSet(void* dest, int c, ptr_size n) {
    uchar* p = (uchar*)dest;
    uchar x = c & 0xff;
    while (n--) {
        *p++ = x;
    }
    return dest;
}

} // namespace core
