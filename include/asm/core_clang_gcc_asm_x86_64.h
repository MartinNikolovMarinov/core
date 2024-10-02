#pragma once

#include <core_types.h>
#include <core_system_checks.h>

namespace core
{

using namespace coretypes;

// Add and set byte if no overflow
inline bool x86_asm_add_setno(char a, char b, char& out) {
    u8 overflow;
    asm volatile (
        "addb %2, %1\n\t"            // Add b to a, result in a
        "movb %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i8 a, i8 b, i8& out) {
    u8 overflow;
    asm volatile (
        "addb %2, %1\n\t"            // Add b to a, result in a
        "movb %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i16 a, i16 b, i16& out) {
    u8 overflow;
    asm volatile (
        "addw %2, %1\n\t"            // Add b to a, result in a
        "movw %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i32 a, i32 b, i32& out) {
    u8 overflow;
    asm volatile (
        "addl %2, %1\n\t"            // Add b to a, result in a
        "movl %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i64 a, i64 b, i64& out) {
    u8 overflow;
    asm volatile (
        "addq %2, %1\n\t"            // Add b to a, result in a
        "movq %1, (%3)\n\t"          // Store the result in out
        "setno %0\n\t"               // Set to 1 if no overflow
        : "=r"(overflow), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return overflow;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u8 a, u8 b, u8& out) {
    u8 carry;
    asm volatile (
        "addb %2, %1\n\t"           // Add b to a, result in a
        "movb %1, (%3)\n\t"         // Store the result in out
        "setnc %0\n\t"              // Set to 1 if no carry
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u16 a, u16 b, u16& out) {
    u8 carry;
    asm volatile (
        "addw %2, %1\n\t"           // Add b to a, result in a
        "movw %1, (%3)\n\t"         // Store the result in out
        "setnc %0\n\t"              // Set to 1 if no carry
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u32 a, u32 b, u32& out) {
    u8 carry;
    asm volatile (
        "addl %2, %1\n\t"           // Add b to a, result in a
        "movl %1, (%3)\n\t"         // Store the result in out
        "setnc %0\n\t"              // Set to 1 if no carry
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u64 a, u64 b, u64& out) {
    u8 carry;
    asm volatile (
        "addq %2, %1\n\t"           // Add b to a, result in a
        "movq %1, (%3)\n\t"         // Store the result in out
        "setnc %0\n\t"              // Set to 1 if no carry
        : "=r"(carry), "+r"(a)
        : "r"(b), "r"(&out)
        : "cc", "memory"
    );
    out = a;
    return carry;
}

} // namespace core
