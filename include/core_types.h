#pragma once

/*
    IMPORTANT: It's best to avoid adding any includes to this file to avoid cyclic dependencies, because it is
    literarily included in every other file. This code will likely be used in a 'using namspace coretypes' statement,
    which is the reason it is in a namespace separate from the rest of the code.
*/

// Include system defaults.
#include <stdint.h>
#include <stddef.h>

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

static constexpr i32 BYTE_SIZE = 8;

// Storage Sizes
static constexpr u64 CORE_BYTE     = static_cast<u64>(1);
static constexpr u64 CORE_KILOBYTE = static_cast<u64>(1024 * CORE_BYTE);
static constexpr u64 CORE_MEGABYTE = static_cast<u64>(1024 * CORE_KILOBYTE);
static constexpr u64 CORE_GIGABYTE = static_cast<u64>(1024 * CORE_MEGABYTE);
static constexpr u64 CORE_TERABYTE = static_cast<u64>(1024 * CORE_GIGABYTE);

// Duration constants in ns
static constexpr u64 CORE_NANOSECOND  = static_cast<u64>(1);                       //                 1ns
static constexpr u64 CORE_MICROSECOND = static_cast<u64>(1000 * CORE_NANOSECOND);  //             1_000ns
static constexpr u64 CORE_MILLISECOND = static_cast<u64>(1000 * CORE_MICROSECOND); //         1_000_000ns
static constexpr u64 CORE_SECOND      = static_cast<u64>(1000 * CORE_MILLISECOND); //     1_000_000_000ns
static constexpr u64 CORE_MINUTE      = static_cast<u64>(60 * CORE_SECOND);        //    60_000_000_000ns
static constexpr u64 CORE_HOUR        = static_cast<u64>(60 * CORE_MINUTE);        // 3_600_000_000_000ns

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
