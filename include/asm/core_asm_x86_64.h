#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_system_checks.h>
#include <core_macros.h>

namespace core
{

using namespace coretypes;

CORE_API_EXPORT bool x86_asm_add_no_overflow(char a, char b, char& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(i8 a, i8 b, i8& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(i16 a, i16 b, i16& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(i32 a, i32 b, i32& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(i64 a, i64 b, i64& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(u8 a, u8 b, u8& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(u16 a, u16 b, u16& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(u32 a, u32 b, u32& out);
CORE_API_EXPORT bool x86_asm_add_no_overflow(u64 a, u64 b, u64& out);

CORE_API_EXPORT bool x86_asm_sub_no_overflow(char a, char b, char& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(i8 a, i8 b, i8& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(i16 a, i16 b, i16& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(i32 a, i32 b, i32& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(i64 a, i64 b, i64& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(u8 a, u8 b, u8& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(u16 a, u16 b, u16& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(u32 a, u32 b, u32& out);
CORE_API_EXPORT bool x86_asm_sub_no_overflow(u64 a, u64 b, u64& out);

CORE_API_EXPORT bool x86_asm_mul_overflow(char a, char b, char& out);
CORE_API_EXPORT bool x86_asm_mul_overflow(i8 a, i8 b, i8 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(i16 a, i16 b, i16 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(i32 a, i32 b, i32 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(i64 a, i64 b, i64 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(u8 a, u8 b, u8 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(u16 a, u16 b, u16 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(u32 a, u32 b, u32 &out);
CORE_API_EXPORT bool x86_asm_mul_overflow(u64 a, u64 b, u64 &out);

} // namespace core
