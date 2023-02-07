#pragma once

#include <API.h>
#include <types.h>

namespace core {

using namespace coretypes;

constexpr u64 pow_u64(i64 n, u32 p) {
    // TODO: Implement faster. I could use some approximating algorithm for very fast pow function.
    //       This might actually be worse than using a for loop?
    if (n == 0) return 0;
    if (p == 0) return 1;
    u64 result = 1;
    while(p > 0) {
        if((p & 1) != 0) result = (result * n);
        n = (n * n);
        p = p >> 1; // p^2
    }
    return result;
}

constexpr f32 deg_to_rad(f32 n) {
    return n * (PI / 180.0f);
}

constexpr f32 rad_to_deg(f32 n) {
    return n * (180.0f / PI);
}

template <typename T>
constexpr T max(T a, T b) {
    // can be done branchless, but it's not faster.
    return a > b ? a : b;
}

template <typename T>
constexpr T min(T a, T b) {
    // can be done branchless, but it's not faster.
    return a < b ? a : b;
}

// NOTE: This is pretty fast branchless check Its collapsed to a single instruction on x86 and ARM by most compilers.
constexpr f32 abs(f32 a) {
    i32* ip = reinterpret_cast<i32*>(&a);
    *ip &= 0x7fffffff;
    return a;
}

constexpr f64 abs(f64 a) {
    i64* ip = reinterpret_cast<i64*>(&a);
    *ip &= 0x7fffffffffffffff;
    return a;
}

constexpr i8 abs(i8 a) {
    // FIXME: find a better way to do this!
    u32 temp = a >> (sizeof(i8) * 8 - 1);
    a ^= temp;
    a += temp & 1;
    return a;
}

constexpr i16 abs(i16 a) {
    u32 temp = a >> (sizeof(i16) * 8 - 1);
    a ^= temp;
    a += temp & 1;
    return a;
}

constexpr i32 abs(i32 a) {
    u32 temp = a >> (sizeof(i32) * 8 - 1);
    a ^= temp;
    a += temp & 1;
    return a;
}

constexpr i64 abs(i64 a) {
    u64 temp = a >> (sizeof(i64) * 8 - 1);
    a ^= temp;
    a += temp & 1;
    return a;
}

// NOTE: This is pretty fast branchless check. Its collapsed to a single instruction on x86 and ARM by most compilers.
constexpr bool is_positive(f32 a) {
    i32* ip = reinterpret_cast<i32*>(&a);
    *ip = (*ip >> 31) << 1;
    return *ip == 0;
}

constexpr bool is_positive(f64 a) {
    i64* ip = reinterpret_cast<i64*>(&a);
    *ip = (*ip >> 63) << 1;
    return *ip == 0;
}

constexpr bool is_positive(i8 a) {
    u32 temp = a >> (sizeof(i8) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i16 a) {
    u32 temp = a >> (sizeof(i16) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i32 a) {
    u32 temp = a >> (sizeof(i32) * 8 - 1);
    return temp == 0;
}

constexpr bool is_positive(i64 a) {
    u64 temp = a >> (sizeof(i64) * 8 - 1);
    return temp == 0;
}

constexpr bool safe_eq(f32 a, f32 b, f32 epsilon) {
    return core::abs(a - b) < epsilon;
}

constexpr bool safe_eq(f64 a, f64 b, f64 epsilon) {
    return core::abs(a - b) < epsilon;
}

} // namespace core
