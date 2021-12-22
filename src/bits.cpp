#include "core.h"

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

template <typename TUint>
TUint SwapByteOrder(TUint n) {
    if constexpr (sizeof(TUint) == 2) {
        u16 swapped = (n >> 8) | (n << 8);
        return swapped;
    } else if constexpr (sizeof(TUint) == 4) {
        u32 b3Tob0 = (n >> 24) & 0xff;       // move byte 3 to byte 0
        u32 b1Tob2 = (n << 8)  & 0xff0000;   // move byte 1 to byte 2
        u32 b2Tob1 = (n >> 8)  & 0xff00;     // move byte 2 to byte 1
        u32 b0Tob3 = (n << 24) & 0xff000000; // byte 0 to byte 3

        // OR them together and the resulting number has a reversed byte order.
        u32 swapped = b3Tob0 | b1Tob2 | b2Tob1 | b0Tob3;
        return swapped;
    } else {
        static_assert((sizeof(TUint) == 2 || sizeof(TUint) == 4), "Invalid TUint argument.");
        return 0;
    }
}

template <typename TFloat>
void FloatToBin(u8 bytes[sizeof(TFloat)], TFloat v) {
    static_assert((sizeof(TFloat) == 4 || sizeof(TFloat) == 8), "Invalid TFloat argument.");
    union { TFloat a; u8 bytes[sizeof(TFloat)]; } floatUnion;
    floatUnion.a = v;
    MemCopy(bytes, floatUnion.bytes, sizeof(TFloat));
}

} // namespace core

