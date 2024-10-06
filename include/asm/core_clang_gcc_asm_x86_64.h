#pragma once

#include <core_types.h>
#include <core_system_checks.h>

namespace core
{

using namespace coretypes;

#pragma region ADD and SETNO/SETNC

inline bool x86_asm_add_setno(char a, char b, char& out) {
    bool overflow;
    asm volatile (
        "addb %2, %1\n\t"            // Add b to a, result in a
        "movb %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)    // output: overflow, modified a
        : "r"(b), "r"(&out)          // input: b, address of out
        : "cc", "memory"             // clobber flags and memory
    );
    return overflow;
}

inline bool x86_asm_add_setno(i8 a, i8 b, i8& out) {
    bool overflow;
    asm volatile (
        "addb %2, %1\n\t"
        "movb %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_add_setno(i16 a, i16 b, i16& out) {
    bool overflow;
    asm volatile (
        "addw %2, %1\n\t"
        "movw %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_add_setno(i32 a, i32 b, i32& out) {
    bool overflow;
    asm volatile (
        "addl %2, %1\n\t"
        "movl %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_add_setno(i64 a, i64 b, i64& out) {
    bool overflow;
    asm volatile (
        "addq %2, %1\n\t"
        "movq %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_add_setnc(u8 a, u8 b, u8& out) {
    bool carry;
    asm volatile (
        "addb %2, %1\n\t"
        "movb %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_add_setnc(u16 a, u16 b, u16& out) {
    bool carry;
    asm volatile (
        "addw %2, %1\n\t"
        "movw %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_add_setnc(u32 a, u32 b, u32& out) {
    bool carry;
    asm volatile (
        "addl %2, %1\n\t"
        "movl %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_add_setnc(u64 a, u64 b, u64& out) {
    bool carry;
    asm volatile (
        "addq %2, %1\n\t"
        "movq %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

#pragma endregion

#pragma region SUB and SETNO/SETNC

inline bool x86_asm_sub_setno(char a, char b, char& out) {
    bool overflow;
    asm volatile (
        "subb %2, %1\n\t"            // Sub b from a, result in a
        "movb %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)    // output: overflow, modified a
        : "r"(b), "r"(&out)          // input: b, address of out
        : "cc", "memory"             // clobber flags and memory
    );
    return overflow;
}

inline bool x86_asm_sub_setno(i8 a, i8 b, i8& out) {
    bool overflow;
    asm volatile (
        "subb %2, %1\n\t"
        "movb %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_sub_setno(i16 a, i16 b, i16& out) {
    bool overflow;
    asm volatile (
        "subw %2, %1\n\t"
        "movw %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_sub_setno(i32 a, i32 b, i32& out) {
    bool overflow;
    asm volatile (
        "subl %2, %1\n\t"
        "movl %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_sub_setno(i64 a, i64 b, i64& out) {
    bool overflow;
    asm volatile (
        "subq %2, %1\n\t"
        "movq %1, (%3)\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return overflow;
}

inline bool x86_asm_sub_setnc(u8 a, u8 b, u8& out) {
    bool carry;
    asm volatile (
        "subb %2, %1\n\t"
        "movb %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_sub_setnc(u16 a, u16 b, u16& out) {
    bool carry;
    asm volatile (
        "subw %2, %1\n\t"
        "movw %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_sub_setnc(u32 a, u32 b, u32& out) {
    bool carry;
    asm volatile (
        "subl %2, %1\n\t"
        "movl %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

inline bool x86_asm_sub_setnc(u64 a, u64 b, u64& out) {
    bool carry;
    asm volatile (
        "subq %2, %1\n\t"
        "movq %1, (%3)\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    return carry;
}

#pragma endregion

} // namespace core
