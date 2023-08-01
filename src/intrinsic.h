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

namespace detail {

template<typename TInt>
constexpr u32 leading_zero_count_compiletime_impl(TInt n) {
    u32 leadingZeroes = sizeof(n) * 8;
    for (i32 i = 0; i < i32(sizeof(n) * 8); i++) {
        leadingZeroes--;
        n = n >> 1;
        if (n == 0) break;
    }
    return leadingZeroes;
}

template<typename TInt>
constexpr u32 i_leading_zero_count(TInt n) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    if (n == 0) return 0; // The gnu gcc __builtin_clz and __builtin_clzll documentation states that n = 0 is undefined behavior!

    if (core::is_const_evaluated()) {
        return detail::leading_zero_count_compiletime_impl(n);
    }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    return (sizeof(TInt) == 4) ? u32(__builtin_clz(n)) : u32(__builtin_clzll(n));
#elif COMPILER_MSVC == 1
    return (sizeof(TInt) == 4) ? __lzcnt(u32(n)) : u32(__lzcnt64(u64(n)));
#else
    return detail::leading_zero_count_compiletime_impl(n);
#endif
}

} // detail namespace

constexpr u32 i_leading_zero_count(u32 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(u64 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(i32 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(i64 n) { return detail::i_leading_zero_count(n); }
template<typename Invalid> u32 i_leading_zero_count(Invalid) {
    static_assert(core::always_false<Invalid>, "Invalid type passed to i_leading_zero_count");
    return 0;
}

constexpr f32 i_huge_valf() { return __builtin_huge_valf(); }
constexpr f32 i_nanf()      { return __builtin_nanf(""); }
constexpr f32 i_nansf()     { return __builtin_nansf(""); }

constexpr f64 i_huge_val() { return __builtin_huge_val(); }
constexpr f64 i_nan()      { return __builtin_nan(""); }
constexpr f64 i_nans()     { return __builtin_nans(""); }

CORE_API_EXPORT u64 get_cpu_ticks();

} // namespace core
