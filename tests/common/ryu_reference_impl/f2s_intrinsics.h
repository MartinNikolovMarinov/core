#pragma once

#include <core_compiler.h>

PRAGMA_WARNING_PUSH
PRAGMA_WARNING_SUPPRESS_ALL

#include "common.h"
#include "f2s_full_table.h"

namespace ryu {

static inline uint32_t pow5factor_32(uint32_t value) {
    uint32_t count = 0;
    for (;;) {
        assert(value != 0);
        const uint32_t q = value / 5;
        const uint32_t r = value % 5;
        if (r != 0) {
            break;
        }
        value = q;
        ++count;
    }
    return count;
}

// Returns true if value is divisible by 5^p.
static inline bool multipleOfPowerOf5_32(const uint32_t value, const uint32_t p) {
    return pow5factor_32(value) >= p;
}

// Returns true if value is divisible by 2^p.
static inline bool multipleOfPowerOf2_32(const uint32_t value, const uint32_t p) {
    // __builtin_ctz doesn't appear to be faster here.
    return (value & ((1u << p) - 1)) == 0;
}

// It seems to be slightly faster to avoid uint128_t here, although the
// generated code for uint128_t looks slightly nicer.
static inline uint32_t mulShift32(const uint32_t m, const uint64_t factor, const int32_t shift) {
    assert(shift > 32);

    // The casts here help MSVC to avoid calls to the __allmul library
    // function.
    const uint32_t factorLo = (uint32_t)(factor);
    const uint32_t factorHi = (uint32_t)(factor >> 32);
    const uint64_t bits0 = (uint64_t)m * factorLo;
    const uint64_t bits1 = (uint64_t)m * factorHi;

    const uint64_t sum = (bits0 >> 32) + bits1;
    const uint64_t shiftedSum = sum >> (shift - 32);
    assert(shiftedSum <= UINT32_MAX);
    return (uint32_t) shiftedSum;
}

static inline uint32_t mulPow5InvDivPow2(const uint32_t m, const uint32_t q, const int32_t j) {
    return mulShift32(m, FLOAT_POW5_INV_SPLIT[q], j);
}

static inline uint32_t mulPow5divPow2(const uint32_t m, const uint32_t i, const int32_t j) {
    return mulShift32(m, FLOAT_POW5_SPLIT[i], j);
}

} // namespace ryu

PRAGMA_WARNING_POP
