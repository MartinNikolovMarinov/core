#pragma once

#include <API.h>
#include <types.h>
#include <mem.h>

namespace core
{

using namespace coretypes;

// Least Significant N Bits are equal to bitSeq
CORE_API_EXPORT bool LSNBits(u8 v, u8 bitSeq, u8 n);
// Most Significant N Bits are equal to bitSeq
CORE_API_EXPORT bool MSNBits(u8 v, u8 bitSeq, u8 n);

template <typename TUint>
CORE_API_EXPORT TUint SwapByteOrder(TUint n) {
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
CORE_API_EXPORT void FloatToBin(u8 bytes[sizeof(TFloat)], TFloat v) {
    static_assert((sizeof(TFloat) == 4 || sizeof(TFloat) == 8), "Invalid TFloat argument.");
    union { TFloat a; u8 bytes[sizeof(TFloat)]; } floatUnion;
    floatUnion.a = v;
    MemCopy(bytes, floatUnion.bytes, sizeof(TFloat));
}

} // namespace core
