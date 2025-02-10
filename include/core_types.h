#pragma once

/*
    IMPORTANT: It's best to avoid adding any includes to this file to avoid cyclic dependencies, because it is
    literarily included in every other file. This code will likely be used in a 'using namspace coretypes' statement,
    which is the reason it is in a namespace separate from the rest of the code.
*/

// Include system defaults.
#include <stdint.h>
#include <stddef.h>

#include <type_traits>
#include <core_compiler.h>

namespace coretypes {

using i8        = int8_t;
using i16       = int16_t;
using i32       = int32_t;
using i64       = int64_t;
using u8        = uint8_t;
using u16       = uint16_t;
using u32       = uint32_t;
using u64       = uint64_t;
using f32       = float;
using f64       = double;
using uchar     = unsigned char;
using schar     = signed char;
using addr_size = u64;
using addr_off  = i64;
using rune      = u32; // Runes represent a single UTF-32 encoded character.

template <typename T>
struct rawbytes { u8 bytes[sizeof(T)]; };

static constexpr i32 BYTE_SIZE = 8;

// Storage Sizes
static constexpr u64 CORE_BYTE     = u64(1);
static constexpr u64 CORE_KILOBYTE = u64(1024 * CORE_BYTE);
static constexpr u64 CORE_MEGABYTE = u64(1024 * CORE_KILOBYTE);
static constexpr u64 CORE_GIGABYTE = u64(1024 * CORE_MEGABYTE);
static constexpr u64 CORE_TERABYTE = u64(1024 * CORE_GIGABYTE);

// Duration constants in ns
static constexpr u64 CORE_NANOSECOND  = u64(1);                       //                 1ns
static constexpr u64 CORE_MICROSECOND = u64(1000 * CORE_NANOSECOND);  //             1_000ns
static constexpr u64 CORE_MILLISECOND = u64(1000 * CORE_MICROSECOND); //         1_000_000ns
static constexpr u64 CORE_SECOND      = u64(1000 * CORE_MILLISECOND); //     1_000_000_000ns
static constexpr u64 CORE_MINUTE      = u64(60 * CORE_SECOND);        //    60_000_000_000ns
static constexpr u64 CORE_HOUR        = u64(60 * CORE_MINUTE);        // 3_600_000_000_000ns

template <typename T>
constexpr T limitMax() {
    if constexpr (std::is_same_v<T, u8>)       return u8(0xFF);                     // 255
    else if constexpr (std::is_same_v<T, u16>) return u16(0xFFFF);                  // 65535
    else if constexpr (std::is_same_v<T, u32>) return u32(0xFFFFFFFF);              // 4294967295
    else if constexpr (std::is_same_v<T, u64>) return u64(0xFFFFFFFFFFFFFFFF);      // 18446744073709551615
    else if constexpr (std::is_same_v<T, i8>)  return i8(0x7F);                     // 127
    else if constexpr (std::is_same_v<T, i16>) return i16(0x7FFF);                  // 32767
    else if constexpr (std::is_same_v<T, i32>) return i32(0x7FFFFFFF);              // 2147483647
    else if constexpr (std::is_same_v<T, i64>) return i64(0x7FFFFFFFFFFFFFFF);      // 9223372036854775807
    else if constexpr (std::is_same_v<T, f32>) return f32(0x1.fffffep+127);         // Maximum positive normal f32
    else if constexpr (std::is_same_v<T, f64>) return f64(0x1.fffffffffffffp+1023); // Maximum positive normal f64
}

template <typename T>
constexpr T limitMin() {
    PRAGMA_WARNING_PUSH
    DISABLE_MSVC_WARNING(4146)

    if constexpr (std::is_same_v<T, u8>)       return u8(0x0);                  // 0
    else if constexpr (std::is_same_v<T, u16>) return u16(0x0);                 // 0
    else if constexpr (std::is_same_v<T, u32>) return u32(0x0);                 // 0
    else if constexpr (std::is_same_v<T, u64>) return u64(0x0);                 // 0
    else if constexpr (std::is_same_v<T, i8>)  return i8(-0x80);                // -128
    else if constexpr (std::is_same_v<T, i16>) return i16(-0x8000);             // -32768
    else if constexpr (std::is_same_v<T, i32>) return i32(-0x80000000);         // -2147483648
    else if constexpr (std::is_same_v<T, i64>) return i64(-0x8000000000000000); // -9223372036854775808
    else if constexpr (std::is_same_v<T, f32>) return f32(0x1.0p-126);          // Minimum positive subnormal f32
    else if constexpr (std::is_same_v<T, f64>) return f64(0x1.0p-1022);         // Minimum positive subnormal f64

    PRAGMA_WARNING_POP
}

// TODO2: If I need the lowest normal f32 it's just equal to "-limitMax<TFloat>()"

} // namespace coretypes

// Sanity static asserts
static_assert(sizeof(coretypes::i8) == 1, "i8 is not 1 byte");
static_assert(sizeof(coretypes::i16) == 2, "i16 is not 2 bytes");
static_assert(sizeof(coretypes::i32) == 4, "i32 is not 4 bytes");
static_assert(sizeof(coretypes::i64) == 8, "i64 is not 8 bytes");

static_assert(sizeof(coretypes::u8) == 1, "u8 is not 1 byte");
static_assert(sizeof(coretypes::u16) == 2, "u16 is not 2 bytes");
static_assert(sizeof(coretypes::u32) == 4, "u32 is not 4 bytes");
static_assert(sizeof(coretypes::u64) == 8, "u64 is not 8 bytes");

static_assert(sizeof(coretypes::f32) == 4, "f32 is not 4 bytes");
static_assert(sizeof(coretypes::f64) == 8, "f64 is not 8 bytes");

static_assert(sizeof(coretypes::addr_size) == 8, "addr_size is not 8 bytes");
static_assert(sizeof(coretypes::addr_off) == 8, "addr_off is not 8 bytes");

static_assert(sizeof(coretypes::rune) == 4, "rune is not 4 bytes");
static_assert(sizeof(coretypes::uchar) == 1, "uchar is not 1 byte");
static_assert(sizeof(coretypes::schar) == 1, "schar is not 1 byte");
