#pragma once

#include <core_API.h>
#include <core_system_checks.h>
#include <core_types.h>
#include <core_traits.h>

#include <asm/core_asm.h>

#if COMPILER_MSVC == 1
#include <intrin.h>
#endif

namespace core {

using namespace coretypes;

                      constexpr u32        intrin_countLeadingZeros(i8 n);
                      constexpr u32        intrin_countLeadingZeros(i16 n);
                      constexpr u32        intrin_countLeadingZeros(i32 n);
                      constexpr u32        intrin_countLeadingZeros(i64 n);
                      constexpr u32        intrin_countLeadingZeros(u8 n);
                      constexpr u32        intrin_countLeadingZeros(u16 n);
                      constexpr u32        intrin_countLeadingZeros(u32 n);
                      constexpr u32        intrin_countLeadingZeros(u64 n);

                      constexpr f32        intrin_hugeValf();
                      constexpr f32        intrin_nanf();
                      constexpr f32        intrin_nansf();
                      constexpr f64        intrin_hugeVal();
                      constexpr f64        intrin_nan();
                      constexpr f64        intrin_nans();

                      constexpr u32        intrin_numberOfSetBits(u32 n);
                      constexpr u32        intrin_numberOfSetBits(u64 n);

                      constexpr inline u32 intrin_rotl(u32 x, i32 s);
                      constexpr inline u64 intrin_rotl(u64 x, i32 s);

                      constexpr inline u32 intrin_rotr(u32 x, i32 s);
                      constexpr inline u64 intrin_rotr(u64 x, i32 s);

template <typename T> constexpr inline bool intrin_safeAdd(T a, T b, T& out);
template <typename T> constexpr inline bool intrin_safeSub(T a, T b, T& out);
template <typename T> constexpr inline bool intrin_safeMul(T a, T b, T& out);

template <typename To, typename From> constexpr inline To intrin_bitCast(const From& src);

namespace detail {

template<typename TInt>
constexpr u32 leadingZeroCountCompiletimeImpl(TInt n) {
    u32 leadingZeroes = sizeof(n) * core::BYTE_SIZE;
    for (i32 i = 0; i < i32(sizeof(n) * core::BYTE_SIZE); i++) {
        leadingZeroes--;
        n = n >> 1;
        if (n == 0) break;
    }
    return leadingZeroes;
}

template<typename TInt>
constexpr u32 intrin_countLeadingZeros(TInt n) {
    if (n == 0) return sizeof(n) * core::BYTE_SIZE; // __builtin_clz(0) is undefined!

    IS_CONST_EVALUATED { return leadingZeroCountCompiletimeImpl(n); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    if constexpr (sizeof(TInt) < 4) {
        // Cast to u32 and then subtract the extra zeros (32 - (sizeof(TInt)*8))
        return u32(__builtin_clz(u32(n))) - u32(32 - (sizeof(TInt) * 8));
    }
    else if constexpr (sizeof(TInt) == 4) {
        return u32(__builtin_clz(u32(n)));
    }
    else {
        return u32(__builtin_clzll(u64(n)));
    }
#elif COMPILER_MSVC == 1
    if constexpr (sizeof(TInt) < 4) {
        // Cast to u32 and then subtract the extra zeros (32 - (sizeof(TInt)*8))
        return u32(__lzcnt(u32(n))) - u32(32 - (sizeof(TInt) * 8));
    }
    else if constexpr (sizeof(TInt) == 4) {
        // TODO2: [PERFORMANCE] Does _BitScanReverse generate better machine code here?
        return u32(__lzcnt(u32(n)));
    }
    else {
        return u32(__lzcnt64(u64(n)));
    }
#else
    return leadingZeroCountCompiletimeImpl(n);
#endif
}

} // namespace detail

constexpr u32 intrin_countLeadingZeros(i8 n) { return detail::intrin_countLeadingZeros(u8(n)); }
constexpr u32 intrin_countLeadingZeros(i16 n) { return detail::intrin_countLeadingZeros(u16(n)); }
constexpr u32 intrin_countLeadingZeros(i32 n) { return detail::intrin_countLeadingZeros(u32(n)); }
constexpr u32 intrin_countLeadingZeros(i64 n) { return detail::intrin_countLeadingZeros(u64(n)); }
constexpr u32 intrin_countLeadingZeros(u8 n) { return detail::intrin_countLeadingZeros(n); }
constexpr u32 intrin_countLeadingZeros(u16 n) { return detail::intrin_countLeadingZeros(n); }
constexpr u32 intrin_countLeadingZeros(u32 n) { return detail::intrin_countLeadingZeros(n); }
constexpr u32 intrin_countLeadingZeros(u64 n) { return detail::intrin_countLeadingZeros(n); }

constexpr f32 intrin_hugeValf()  { return __builtin_huge_valf(); }
constexpr f32 intrin_nanf()      { return __builtin_nanf(""); }
constexpr f32 intrin_nansf()     { return __builtin_nansf(""); }

constexpr f64 intrin_hugeVal()  { return __builtin_huge_val(); }
constexpr f64 intrin_nan()      { return __builtin_nan(""); }
constexpr f64 intrin_nans()     { return __builtin_nans(""); }

namespace detail {

template<typename TUint>
constexpr u32 numberOfSetBitsCompiletimeImpl(TUint n) {
    u32 setBits = 0;
    for (i32 i = 0; i < i32(sizeof(n) * core::BYTE_SIZE); i++) {
        if (n & 1) setBits++;
        n = n >> 1;
    }
    return setBits;
}

template<typename TUint>
constexpr u32 intrin_numberOfSetBits(TUint n) {
    IS_CONST_EVALUATED { return detail::numberOfSetBitsCompiletimeImpl(n); }

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
    return details::numberOfSetBitsCompiletimeImpl(n);
#endif
}

} // namespace detail

constexpr u32 intrin_numberOfSetBits(u32 n) { return detail::intrin_numberOfSetBits(n); }
constexpr u32 intrin_numberOfSetBits(u64 n) { return detail::intrin_numberOfSetBits(n); }

namespace detail {

template<typename TUint>
constexpr inline TUint intrin_rotl(TUint x, i32 s) {
    constexpr i32 N = sizeof(TUint) * core::BYTE_SIZE;
    auto r = s % N;
    if (r == 0)     return x;
    else if (r > 0) return (x << r) | (x >> (N - r));
    else            return (x >> (-r)) | (x << (N + r));
}

}

// Left circular shift.
constexpr inline u32 intrin_rotl(u32 x, i32 s) { return detail::intrin_rotl(x, s); }
constexpr inline u64 intrin_rotl(u64 x, i32 s) { return detail::intrin_rotl(x, s); }

namespace detail {

template<typename TUint>
constexpr inline TUint intrin_rotr(TUint x, i32 s) {
    constexpr i32 N = sizeof(TUint) * core::BYTE_SIZE;
    auto r = s % N;
    if (r == 0)     return x;
    else if (r > 0) return (x >> r) | (x << (N - r));
    else            return (x << (-r)) | (x >> (N + r));
}

}

// Right circular shift.
constexpr inline u32 intrin_rotr(u32 x, i32 s) { return detail::intrin_rotr(x, s); }
constexpr inline u64 intrin_rotr(u64 x, i32 s) { return detail::intrin_rotr(x, s); }

namespace detail {

template <typename T>
constexpr bool safeAddComptimeImpl(T a, T b, T& out) {
    if constexpr (std::is_signed_v<T>) {
        if ((b > 0 && a > core::limitMax<T>() - b) ||
            (b < 0 && a < core::limitMin<T>() - b)) {
            return false;
        }
    }
    else {
        if (a > core::limitMax<T>() - b) {
            return false;
        }
    }

    out = a + b;
    return true;
}

} // namespace detail

template <typename T>
constexpr inline bool intrin_safeAdd(T a, T b, T& out) {
    static_assert(std::is_integral_v<T>);

    IS_CONST_EVALUATED { return detail::safeAddComptimeImpl(a, b, out); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    return !__builtin_add_overflow(a, b, &out);
#elif defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    return core::x86_asm_add_no_overflow(a, b, out);
#else
    // fallback
    return detail::safeAddComptimeImpl(a, b, out);
#endif
}

namespace detail {

template <typename T>
constexpr bool safeSubComptimeImpl(T a, T b, T& out) {
    if constexpr (std::is_signed_v<T>) {
        if ((b > 0 && a < core::limitMin<T>() + b) ||
            (b < 0 && a > core::limitMax<T>() + b)) {
            return false;
        }
    }
    else {
        if (a < b) {
            return false;
        }
    }

    out = a - b;
    return true;
}

} // namespace detail

template <typename T>
constexpr inline bool intrin_safeSub(T a, T b, T& out) {
    static_assert(std::is_integral_v<T>);

    IS_CONST_EVALUATED { return detail::safeSubComptimeImpl(a, b, out); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    return !__builtin_sub_overflow(a, b, &out);
#elif defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    return core::x86_asm_sub_no_overflow(a, b, out);
#else
    // fallback
    return detail::safeSubComptimeImpl(a, b, out);
#endif
}

namespace detail {

template <typename T>
constexpr inline bool safeMulComptimeImpl(T a, T b, T& out) {
    // Handle multiplication by zero separately
    if (a == 0 || b == 0) {
        out = 0;
        return true;
    }

    if constexpr (std::is_signed_v<T>) {
        // Handle special case: multiplying minimum by -1 causes overflow
        if (a == core::limitMin<T>() && b == -1) {
            return false;
        }
        if (b == core::limitMin<T>() && a == -1) {
            return false;
        }

        // Dividing min by -1 can cause arithmetic exception, so skip this check. This is ok, because the only case
        // where the multiplication can overflow is -1 * min and that case was already handled explicitly.
        if (b != -1) {
            if (b > 0) {
                if (a > core::limitMax<T>() / b || a < core::limitMin<T>() / b) {
                    return false;
                }
            }
            else {
                if (a > core::limitMin<T>() / b || a < core::limitMax<T>() / b) {
                    return false;
                }
            }
        }
    }
    else {
        if (a > core::limitMax<T>() / b) {
            return false;
        }
    }

    out = a * b;
    return true;
}

} // namespace detail

template <typename T>
constexpr inline bool intrin_safeMul(T a, T b, T& out) {
    static_assert(std::is_integral_v<T>);

    IS_CONST_EVALUATED { return detail::safeMulComptimeImpl(a, b, out); }

#if COMPILER_CLANG == 1 || COMPILER_GCC == 1
    return !__builtin_mul_overflow(a, b, &out);
#elif defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    return core::x86_asm_mul_overflow(a, b, out);
#else
    // fallback
    return detail::safeMulComptimeImpl(a, b, out);
#endif
}

template <typename To, typename From>
constexpr inline To intrin_bitCast(const From& src) {
    static_assert(sizeof(To) == sizeof(From), "From and To must the of the same length");
    static_assert(std::is_trivially_copyable_v<To>, "To is not trivially copyable");
    static_assert(std::is_trivially_copyable_v<From>, "From is not trivially copyable");
    return __builtin_bit_cast(To, src);
}

} // namespace core
