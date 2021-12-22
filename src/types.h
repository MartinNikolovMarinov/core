#pragma once

/*
    IMPORTANT:
    It's best to avoid adding any includes to this file to avoid cyclic dependencies,
    because it is literarily included in every other file.
    Also, this file is not in the core namespace on purpose.
*/

// Include system defaults.
#include <stdint.h>
#include <stddef.h>
#include <uchar.h>

namespace coretypes
{

using i8       = int8_t;
using i16      = int16_t;
using i32      = int32_t;
using i64      = int64_t;
using u8       = uint8_t;
using u16      = uint16_t;
using u32      = uint32_t;
using u64      = uint64_t;
using f32      = float;
using f64      = double;
using uchar    = unsigned char;
using ptr_size = size_t;
using rune     = char32_t; // Runes represent a single UTF-32 encoded character.

// Max unsigned integer constants
static constexpr u64 MAX_U64 = ((u64)0xFFFFFFFFFFFFFFFF);             // 18446744073709551615
static constexpr u32 MAX_U32 = ((u32)(MAX_U64 >> (sizeof(u32) * 8))); // 4294967295
static constexpr u16 MAX_U16 = ((u16)(MAX_U32 >> (sizeof(u16) * 8))); // 65535
static constexpr u8  MAX_U8  = ((u8)(MAX_U16 >> (sizeof(u8) * 8)));   // 255

// Max signed integer constants
static constexpr i64 MAX_I64 = ((i64)(MAX_U64 >> 1));                 // 9223372036854775807
static constexpr i32 MAX_I32 = ((i32)(MAX_I64 >> (sizeof(i32) * 8))); // 2147483647
static constexpr i16 MAX_I16 = ((i16)(MAX_I32 >> (sizeof(i16) * 8))); // 32767
static constexpr i8  MAX_I8  = ((i8)(MAX_I16 >> (sizeof(i8) * 8)));   // 127

// Min signed integer constants
static constexpr i64 MIN_I64 = ((i64)((MAX_U64 >> 1) ^ MAX_U64)); // -9223372036854775808
static constexpr i32 MIN_I32 = ((i32)((MAX_U32 >> 1) ^ MAX_U32)); // -2147483648
static constexpr i16 MIN_I16 = ((i16)((MAX_U16 >> 1) ^ MAX_U16)); // -32768
static constexpr i8  MIN_I8  = ((i8)((MAX_U8 >> 1) ^ MAX_U8));    // -128

// Special constants
static constexpr char term_char = '\0';

// Standard I/O
static constexpr i32 STDIN  = 0;
static constexpr i32 STDOUT = 1;
static constexpr i32 STDERR = 2;

// Storage Sizes
static constexpr i64 BYTE     = ((i64)1);
static constexpr i64 KILOBYTE = ((i64)(1000 * BYTE));
static constexpr i64 MEGABYTE = ((i64)(1000 * KILOBYTE));
static constexpr i64 GIGABYTE = ((i64)(1000 * MEGABYTE));
static constexpr i64 TERABYTE = ((i64)(1000 * GIGABYTE));

// Duration constants in ns
static constexpr u64 NANOSECOND  = ((u64)1);                    //                 1ns
static constexpr u64 MICROSECOND = ((u64)(1000 * NANOSECOND));  //             1_000ns
static constexpr u64 MILLISECOND = ((u64)(1000 * MICROSECOND)); //         1_000_000ns
static constexpr u64 SECOND      = ((u64)(1000 * MILLISECOND)); //     1_000_000_000ns
static constexpr u64 MINUTE      = ((u64)(60 * SECOND));        //    60_000_000_000ns
static constexpr u64 HOUR        = ((u64)(60 * MINUTE));        // 3_600_000_000_000ns

} // namespace core
