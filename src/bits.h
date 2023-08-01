#pragma once

#include <API.h>
#include <types.h>
#include <mem.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

// Least Significant N Bits are equal to bitSeq
constexpr bool lsn_bits(u8 v, u8 bitSeq, u8 n) {
    u8 mask = ~u8(MAX_U8 << n);
    v = (v & mask);
    bool ret = (v == bitSeq);
    return ret;
}

// Most Significant N Bits are equal to bitSeq
constexpr bool msn_bits(u8 v, u8 bitSeq, u8 n) {
    u8 shift = (sizeof(u8) * 8) - n;
    bool ret = (v >> shift) == bitSeq;
    return ret;
}

void CORE_API_EXPORT float_to_bin(u8 bytes[sizeof(f32)], f32 v);
void CORE_API_EXPORT float_to_bin(u8 bytes[sizeof(f64)], f64 v);

} // namespace core
