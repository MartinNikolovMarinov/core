#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_intrinsics.h>

namespace core {

using namespace coretypes;

constexpr u32 digitCount(u32 n);
constexpr u32 digitCount(i32 n);
constexpr u32 digitCount(u64 n);
constexpr u32 digitCount(i64 n);

constexpr u32 digitCount(u32 n) {
    if (n == 0) [[unlikely]] return 1;

    // For unsigned integers this can't be branchless:
    if (n >= 1000000000) [[unlikely]] return 10;

    constexpr u32 powers[] = {
        10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000, 1000000000
    };

    // b: Compute floor(log2(n)) by subtracting the number of leading zeros from 31.
    u32 b = -u32(n > 0) & (31 - intrin_countLeadingZeros(n));

    // a: Multiply b by 77 and divide by 256 to approximate floor(log10(n)). 77/256 is approximately ln(2)/ln(10) ≈
    // 0.301, which scales the binary exponent to an approximate decimal digit count.
    u32 a = (b * 77) / 256;

    // Finally, 1 + a gives the candidate digit count; we add 1 more if n is at least as large as 10^(a) (as stored in
    // the lookup table).
    return 1 + a + (n >= powers[a]);
}

constexpr u32 digitCount(i32 n) {
    if (n == 0) [[unlikely]]                     return 1;
    if (n == core::limitMin<i32>()) [[unlikely]] return 10;
    if (n < 0) n = -n;
    return digitCount(u32(n));
}

constexpr u32 digitCount(u64 n) {
    if (n == 0) [[unlikely]] return 1;

    constexpr u64 powers[] = {
        10ull, 100ull, 1000ull, 10000ull, 100000ull, 1000000ull, 10000000ull,
        100000000ull, 1000000000ull, 10000000000ull, 100000000000ull,
        1000000000000ull, 10000000000000ull, 100000000000000ull,
        1000000000000000ull, 10000000000000000ull, 100000000000000000ull,
        1000000000000000000ull, 10000000000000000000ull,
    };

    // b: Compute floor(log2(n)) by subtracting the number of leading zeros from 63.
    u32 b = -u32(n > 0) & (63 - intrin_countLeadingZeros(n));

    // a: Multiply b by 77 and divide by 256 to approximate floor(log10(n)). 77/256 is approximately ln(2)/ln(10) ≈
    // 0.301, which scales the binary exponent to an approximate decimal digit count.
    u32 a = (b * 77) / 256;

    // Finally, 1 + a gives the candidate digit count; we add 1 more if n is at least as large as 10^(a) (as stored in
    // the lookup table).
    return 1 + a + (n >= powers[a]);
}

constexpr u32 digitCount(i64 n) {
    if (n == 0) [[unlikely]]                     return 1;
    if (n == core::limitMin<i64>()) [[unlikely]] return 19;
    if (n < 0) n = -n;
    return digitCount(u64(n));
}

} // namespace core
