#pragma once

#include <core_API.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

                constexpr bool leastSignificantNBits(u8 v, u8 bitSeq, u8 n);
                constexpr bool mostSignificantNBits(u8 v, u8 bitSeq, u8 n);
CORE_API_EXPORT void           floatToBin(u8 bytes[sizeof(f32)], f32 v);
CORE_API_EXPORT void           floatToBin(u8 bytes[sizeof(f64)], f64 v);


// Least Significant N Bits are equal to bitSeq
constexpr bool leastSignificantNBits(u8 v, u8 bitSeq, u8 n) {
    u8 mask = ~u8(core::limitMax<u8>() << n);
    v = (v & mask);
    bool ret = (v == bitSeq);
    return ret;
}

// Most Significant N Bits are equal to bitSeq
constexpr bool mostSignificantNBits(u8 v, u8 bitSeq, u8 n) {
    u8 shift = (sizeof(u8) * 8) - n;
    bool ret = (v >> shift) == bitSeq;
    return ret;
}

} // namespace core
