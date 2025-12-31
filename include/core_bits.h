#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_traits.h>
#include <core_intrinsics.h>

namespace core {

using namespace coretypes;

constexpr bool leastSignificantNBits(u8 v, u8 bitSeq, u8 n);
constexpr bool mostSignificantNBits(u8 v, u8 bitSeq, u8 n);

template <typename To, typename From>
constexpr inline To bitCast(const From& src) noexcept;

template <typename To, typename From>
constexpr inline To bitCast(const From& src) noexcept {
    return intrin_bitCast<To, From>(src);
}

// Least Significant N Bits are equal to bitSeq
constexpr bool leastSignificantNBits(u8 v, u8 bitSeq, u8 n) {
    u8 mask = u8(~u8(core::limitMax<u8>() << n));
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
