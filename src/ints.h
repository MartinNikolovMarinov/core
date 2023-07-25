#pragma once

#include <API.h>
#include <types.h>
#include <intrinsic.h>

namespace core {

using namespace coretypes;

namespace detail {

// Lookup tables for DigitCount:
static constexpr u64 powers[] = {
    0,
    u64(1e1),  u64(1e2),  u64(1e3),  u64(1e4),  u64(1e5),
    u64(1e6),  u64(1e7),  u64(1e8),  u64(1e9),  u64(1e10),
    u64(1e11), u64(1e12), u64(1e13), u64(1e14), u64(1e15),
    u64(1e16), u64(1e17), u64(1e18), u64(1e19)
};
static constexpr u32 maxdigits[] = {
    1, 1, 1, 1,
    2, 2, 2,
    3, 3, 3,
    4, 4, 4, 4,
    5, 5, 5,
    6, 6, 6,
    7, 7, 7, 7,
    8, 8, 8,
    9, 9, 9,
    10, 10, 10,
    11, 11, 11, 11,
    12, 12, 12,
    13, 13, 13,
    14, 14, 14, 14,
    15, 15, 15,
    16, 16, 16,
    17, 17, 17, 17,
    18, 18, 18,
    19, 19, 19,
    20, 20, 20, 20,
};

} // namespace detail

template <typename TUint>
constexpr u32 digit_count(TUint n) {
    static_assert(sizeof(n) == 4 || sizeof(n) == 8, "Invalid TUint paramater.");
    if (n == 0) return 1;
    if (n < 0) n = -n;
    u32 leadingZeroes = leading_zero_count(n);
    u32 usedBits = (sizeof(n) * 8) - u32(leadingZeroes);
    u32 digits = detail::maxdigits[usedBits];
    if (n < static_cast<TUint>(detail::powers[digits - 1])) digits--;
    return digits;
}

} // namespace core
