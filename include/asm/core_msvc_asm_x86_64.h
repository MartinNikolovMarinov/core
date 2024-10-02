#pragma once

#include <core_types.h>

namespace core
{

using namespace coretypes;

// Add and set byte if no overflow
inline bool x86_asm_add_setno(char a, char b, char& out) {
    u8 overflow;
    __asm {
        mov al, a             // Move `a` into `al`
        add al, b             // Add `b` to `al`
        mov out, al           // Store the result in `out`
        setno overflow        // Set overflow to 1 if no overflow
    }
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i8 a, i8 b, i8& out) {
    u8 overflow;
    __asm {
        mov al, a             // Move `a` into `al`
        add al, b             // Add `b` to `al`
        mov out, al           // Store the result in `out`
        setno overflow        // Set overflow to 1 if no overflow
    }
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i16 a, i16 b, i16& out) {
    u8 overflow;
    __asm {
        mov ax, a             // Move `a` into `ax`
        add ax, b             // Add `b` to `ax`
        mov out, ax           // Store the result in `out`
        setno overflow        // Set overflow to 1 if no overflow
    }
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i32 a, i32 b, i32& out) {
    u8 overflow;
    __asm {
        mov eax, a            // Move `a` into `eax`
        add eax, b            // Add `b` to `eax`
        mov out, eax          // Store the result in `out`
        setno overflow        // Set overflow to 1 if no overflow
    }
    return overflow;
}

// Add and set byte if no overflow
inline bool x86_asm_add_setno(i64 a, i64 b, i64& out) {
    u8 overflow;
    __asm {
        mov rax, a            // Move `a` into `rax`
        add rax, b            // Add `b` to `rax`
        mov out, rax          // Store the result in `out`
        setno overflow        // Set overflow to 1 if no overflow
    }
    return overflow;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u8 a, u8 b, u8& out) {
   u8 carry;
    __asm {
        mov al, a             // Move `a` into `al`
        add al, b             // Add `b` to `al`
        mov out, al           // Store the result in `out`
        setnc carry           // Set carry to 1 if no carry
    }
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u16 a, u16 b, u16& out) {
     u8 carry;
    __asm {
        mov ax, a             // Move `a` into `ax`
        add ax, b             // Add `b` to `ax`
        mov out, ax           // Store the result in `out`
        setnc carry           // Set carry to 1 if no carry
    }
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u32 a, u32 b, u32& out) {
    u8 carry;
    __asm {
        mov eax, a            // Move `a` into `eax`
        add eax, b            // Add `b` to `eax`
        mov out, eax          // Store the result in `out`
        setnc carry           // Set carry to 1 if no carry
    }
    return carry;
}

// Add and set carry if no overflow
inline bool x86_asm_add_setnc(u64 a, u64 b, u64& out) {
    u8 carry;
    __asm {
        mov rax, a            // Move `a` into `rax`
        add rax, b            // Add `b` to `rax`
        mov out, rax          // Store the result in `out`
        setnc carry           // Set carry to 1 if no carry
    }
    return carry;
}

} // namespace core
