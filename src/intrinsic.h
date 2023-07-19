#pragma once

#include <API.h>
#include <system_checks.h>
#include <types.h>
#include <core_traits.h>

#if COMPILER_MSVC == 1
#include <intrin.h>
#endif

namespace core {

using namespace coretypes;

namespace {

template<typename TInt>
constexpr u32 leading_zero_count_fallback(TInt n) {
    u32 leadingZeroes = sizeof(n) * 8;
    for (i32 i = 0; i < i32(sizeof(n) * 8); i++) {
        leadingZeroes--;
        n = n >> 1;
        if (n == 0) break;
    }
    return leadingZeroes;
}

} // namespace

template<typename TInt>
constexpr CORE_API_EXPORT u32 leading_zero_count(TInt n) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    if (n == 0) return 0; // The gnu gcc __builtin_clz and __builtin_clzll documentation states that n = 0 is undefined behavior!

    if (core::is_const_evaluated()) {
        return leading_zero_count_fallback(n);
    }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    if constexpr (sizeof(TInt) == 4) {
        return u32(__builtin_clz(n));
    } else {
        return u32(__builtin_clzll(n));
    }
#elif COMPILER_MSVC == 1
    if constexpr (sizeof(TInt) == 4) {
        return u32(__lzcnt(n));
    } else {
        return u32(__lzcnt64(n));
    }
#else
    return leading_zero_count_fallback(n);
#endif
}

CORE_API_EXPORT u64 get_cpu_ticks();

} // namespace core
