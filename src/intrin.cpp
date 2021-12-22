#include "core.h"

#if COMPILER_MSVC
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace core
{

namespace
{

template<typename TUint>
u32 LeadingZerosFallback(TUint n) {
    u32 leadingZeroes = sizeof(n) * 8;
    for (i32 i = 0; i < sizeof(n) * 8; i++) {
        leadingZeroes--;
        if (n = n >> 1; n == 0) break;
    }
    return leadingZeroes;
}

} // namespace

template<typename TUint>
u32 LeadingZeros(TUint n) {
    static_assert(sizeof(n) <= 8, "Invalid TUint paramater.");
    if (n == 0) return 0;

#if COMPILER_CLANG || COMPILER_GCC
    if constexpr (sizeof(TUint) <= 4) {
        return u32(__builtin_clz(n));
    } else {
        return u32(__builtin_clzll(n));
    }
#elif COMPILER_MSVC
    if constexpr (sizeof(TUint) <= 4) {
        return u32(__lzcnt(n));
    } else {
        return u32(__lzcnt64(n));
    }
#else
    return LeadingZerosFallback(n);
#endif
}

u64 GetCPUTicks() {
    return __rdtsc();
}

} // namespace core
