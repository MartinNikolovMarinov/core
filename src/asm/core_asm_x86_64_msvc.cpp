#include <asm/core_asm.h>

#include <intrin.h>

namespace core {

#pragma region ADD

extern "C" {
    // These are implemented in the msvc_add_no_overflow.asm file!
    bool x86_asm_add_no_overflow_char(char a, char b, char* out);
    bool x86_asm_add_no_overflow_i16(i16 a, i16 b, i16* out);
    bool x86_asm_add_no_overflow_i32(i32 a, i32 b, i32* out);
    bool x86_asm_add_no_overflow_i64(i64 a, i64 b, i64* out);
}

bool x86_asm_add_no_overflow(char a, char b, char& out) {
    return x86_asm_add_no_overflow_char(a, b, &out);
}

bool x86_asm_add_no_overflow(i8 a, i8 b, i8& out) {
    return x86_asm_add_no_overflow(char(a), char(b), reinterpret_cast<char&>(out));
}

bool x86_asm_add_no_overflow(i16 a, i16 b, i16& out) {
    return x86_asm_add_no_overflow_i16(a, b, &out);
}

bool x86_asm_add_no_overflow(i32 a, i32 b, i32& out) {
    return x86_asm_add_no_overflow_i32(a, b, &out);
}

bool x86_asm_add_no_overflow(i64 a, i64 b, i64& out) {
    return x86_asm_add_no_overflow_i64(a, b, &out);
}

bool x86_asm_add_no_overflow(u8 a, u8 b, u8& out) {
    auto c = _addcarry_u8(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_add_no_overflow(u16 a, u16 b, u16& out) {
    auto c = _addcarry_u16(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_add_no_overflow(u32 a, u32 b, u32& out) {
    auto c = _addcarry_u32(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_add_no_overflow(u64 a, u64 b, u64& out) {
    auto c = _addcarry_u64(0, a, b, &out);
    return (c == 0);
}

#pragma endregion

#pragma region SUB

extern "C" {
    // These are implemented in the msvc_sub_no_overflow.asm file!
    bool x86_asm_sub_no_overflow_char(char a, char b, char* out);
    bool x86_asm_sub_no_overflow_i16(i16 a, i16 b, i16* out);
    bool x86_asm_sub_no_overflow_i32(i32 a, i32 b, i32* out);
    bool x86_asm_sub_no_overflow_i64(i64 a, i64 b, i64* out);
}

bool x86_asm_sub_no_overflow(char a, char b, char& out) {
    return x86_asm_sub_no_overflow_char(a, b, &out);
}

bool x86_asm_sub_no_overflow(i8 a, i8 b, i8& out) {
    return x86_asm_sub_no_overflow(char(a), char(b), reinterpret_cast<char&>(out));
}

bool x86_asm_sub_no_overflow(i16 a, i16 b, i16& out) {
    return x86_asm_sub_no_overflow_i16(a, b, &out);
}

bool x86_asm_sub_no_overflow(i32 a, i32 b, i32& out) {
    return x86_asm_sub_no_overflow_i32(a, b, &out);
}

bool x86_asm_sub_no_overflow(i64 a, i64 b, i64& out) {
    return x86_asm_sub_no_overflow_i64(a, b, &out);
}

bool x86_asm_sub_no_overflow(u8 a, u8 b, u8& out) {
    auto c = _subborrow_u8(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_sub_no_overflow(u16 a, u16 b, u16& out) {
    auto c = _subborrow_u16(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_sub_no_overflow(u32 a, u32 b, u32& out) {
    auto c = _subborrow_u32(0, a, b, &out);
    return (c == 0);
}

bool x86_asm_sub_no_overflow(u64 a, u64 b, u64& out) {
    auto c = _subborrow_u64(0, a, b, &out);
    return (c == 0);
}

#pragma endregion

#pragma region MUL

extern "C" {
    // These are implemented in the msvc_mul_no_overflow.asm file!
    bool x86_asm_mul_no_overflow_char(char a, char b, char* out);
    bool x86_asm_mul_no_overflow_i16(i16 a, i16 b, i16* out);
    bool x86_asm_mul_no_overflow_i32(i32 a, i32 b, i32* out);
    bool x86_asm_mul_no_overflow_i64(i64 a, i64 b, i64* out);
    bool x86_asm_mul_no_overflow_u8(u8 a, u8 b, u8* out);
    bool x86_asm_mul_no_overflow_u16(u16 a, u16 b, u16* out);
    bool x86_asm_mul_no_overflow_u32(u32 a, u32 b, u32* out);
    bool x86_asm_mul_no_overflow_u64(u64 a, u64 b, u64* out);
}

bool x86_asm_mul_overflow(char a, char b, char& out) { return x86_asm_mul_no_overflow_char(a, b, &out); }
bool x86_asm_mul_overflow(i8 a, i8 b, i8& out) { return x86_asm_mul_overflow(char(a), char(b), reinterpret_cast<char&>(out)); }
bool x86_asm_mul_overflow(i16 a, i16 b, i16& out) { return x86_asm_mul_no_overflow_i16(a, b, &out); }
bool x86_asm_mul_overflow(i32 a, i32 b, i32& out) { return x86_asm_mul_no_overflow_i32(a, b, &out); }
bool x86_asm_mul_overflow(i64 a, i64 b, i64& out) { return x86_asm_mul_no_overflow_i64(a, b, &out); }
bool x86_asm_mul_overflow(u8 a, u8 b, u8& out) { return x86_asm_mul_no_overflow_u8(a, b, &out); }
bool x86_asm_mul_overflow(u16 a, u16 b, u16& out) { return x86_asm_mul_no_overflow_u16(a, b, &out); }
bool x86_asm_mul_overflow(u32 a, u32 b, u32& out) { return x86_asm_mul_no_overflow_u32(a, b, &out); }
bool x86_asm_mul_overflow(u64 a, u64 b, u64& out) { return x86_asm_mul_no_overflow_u64(a, b, &out); }

#pragma endregion

} // namespace core
