#pragma once

#include <API.h>
#include <system_checks.h>
#include <types.h>
#include <core_traits.h>
#include <utils.h>

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
    if (n == 0) return 0; // The gnu gcc __builtin_clz and __builtin_clzll documentation states that n = 0 is undefined behavior!

    IS_CONST_EVALUATED { return detail::leading_zero_count_compiletime_impl(n); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    if constexpr (sizeof(TInt) == 4) {
        return u32(__builtin_clz(n));
    }
    else {
        return u32(__builtin_clzll(n));
    }
#elif COMPILER_MSVC == 1
    if constexpr (sizeof(TInt) == 4) {
        return u32(__lzcnt(u32(n)));
    }
    else {
        return u32(__lzcnt64(u64(n)));
    }
#else
    return detail::leading_zero_count_compiletime_impl(n);
#endif
}

} // namespace detail

constexpr u32 i_leading_zero_count(u32 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(u64 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(i32 n) { return detail::i_leading_zero_count(n); }
constexpr u32 i_leading_zero_count(i64 n) { return detail::i_leading_zero_count(n); }

GUARD_FN_TYPE_DEDUCTION(i_leading_zero_count);

constexpr f32 i_huge_valf() { return __builtin_huge_valf(); }
constexpr f32 i_nanf()      { return __builtin_nanf(""); }
constexpr f32 i_nansf()     { return __builtin_nansf(""); }

constexpr f64 i_huge_val() { return __builtin_huge_val(); }
constexpr f64 i_nan()      { return __builtin_nan(""); }
constexpr f64 i_nans()     { return __builtin_nans(""); }

namespace detail {

template<typename TUint>
constexpr u32 number_of_set_bits_compiletime_impl(TUint n) {
    u32 setBits = 0;
    for (i32 i = 0; i < i32(sizeof(n) * 8); i++) {
        if (n & 1) setBits++;
        n = n >> 1;
    }
    return setBits;
}

template<typename TUint>
constexpr u32 i_number_of_set_bits(TUint n) {
    IS_CONST_EVALUATED { return detail::number_of_set_bits_compiletime_impl(n); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    if constexpr (sizeof(TUint) == 4) {
        return u32(__builtin_popcount(TUint(n)));
    }
    else {
        return u32(__builtin_popcountll(TUint(n)));
    }
#elif COMPILER_MSVC == 1
    if constexpr (sizeof(TUint) == 4) {
        return u32(__popcnt(u32(n)));
    }
    else {
        return u32(__popcnt64(u64(n)));
    }
#else
    return details::number_of_set_bits_compiletime_impl(n);
#endif
}

} // namespace detail

constexpr u32 i_number_of_set_bits(u32 n) { return detail::i_number_of_set_bits(n); }
constexpr u32 i_number_of_set_bits(u64 n) { return detail::i_number_of_set_bits(n); }

GUARD_FN_TYPE_DEDUCTION(i_number_of_set_bits);

CORE_API_EXPORT u64 get_cpu_ticks();

} // namespace core
