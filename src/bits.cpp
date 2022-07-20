#include "bits.h"

namespace core
{

bool LSNBits(u8 v, u8 bitSeq, u8 n) {
    u8 mask = ~(MAX_U8 << n);
    v = (v & mask);
    bool ret = (v == bitSeq);
    return ret;
}

bool MSNBits(u8 v, u8 bitSeq, u8 n) {
    u8 shift = (sizeof(u8) * 8) - n;
    u8 mask = u8((MAX_U8 << shift));
    v = (v & mask) >> shift;
    bool ret = v == bitSeq;
    return ret;
}

} // namespace core

