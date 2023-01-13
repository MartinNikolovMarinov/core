#pragma once

#include <API.h>
#include <system_checks.h>
#include <types.h>

namespace core {

using namespace coretypes;

namespace {

template<typename TUint>
u32 leading_zeros_fallback(TUint n) {
    u32 leadingZeroes = sizeof(n) * 8;
    for (i32 i = 0; i < sizeof(n) * 8; i++) {
        leadingZeroes--;
        if (n = n >> 1; n == 0) break;
    }
    return leadingZeroes;
}

} // namespace

template<typename TUint>
CORE_API_EXPORT u32 leading_zeroes(TUint n) {
    static_assert(sizeof(n) <= 8, "Invalid TUint paramater.");
    if (n == 0) return 0;

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    if constexpr (sizeof(TUint) <= 4) {
        return u32(__builtin_clz(n));
    } else {
        return u32(__builtin_clzll(n));
    }
#elif COMPILER_MSVC == 1
    if constexpr (sizeof(TUint) <= 4) {
        return u32(__lzcnt(n));
    } else {
        return u32(__lzcnt64(n));
    }
#else
    return leading_zeros_fallback(n);
#endif
}

CORE_API_EXPORT u64 get_cpu_ticks();

} // namespace core
