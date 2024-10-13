#include <asm/core_asm.h>

namespace core {

// FIXME: Add more comments to add sub and mul. There is a lot of documentation to do for multiplication especially!
// FIXME: Write the MSVC inline asm code as well.

#pragma region ADD

namespace {

template <typename T>
bool x86_asm_add_no_overflow_signed(T a, T b, T& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "add %1, %3 \n\t"
        "mov %2, %1 \n\t"
        "setno %0 \n\t"
        : "=r"(overflow), "+r"(a), "=m"(out)
        : "r"(b)
    );
    return overflow;
}

template <typename T>
bool x86_asm_add_no_overflow_unsigned(T a, T b, T& out) {
    bool carry;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "add %1, %3 \n\t"
        "mov %2, %1 \n\t"
        "setnc %0 \n\t"
        : "=r"(carry), "+r"(a), "=m"(out)
        : "r"(b)
    );
    return carry;
}

} // namespace


bool x86_asm_add_no_overflow(char a, char b, char& out) { return x86_asm_add_no_overflow_signed(a, b, out); }

bool x86_asm_add_no_overflow(i8 a, i8 b, i8& out) { return x86_asm_add_no_overflow_signed(a, b, out); }

bool x86_asm_add_no_overflow(i16 a, i16 b, i16& out) { return x86_asm_add_no_overflow_signed(a, b, out); }

bool x86_asm_add_no_overflow(i32 a, i32 b, i32& out) { return x86_asm_add_no_overflow_signed(a, b, out); }

bool x86_asm_add_no_overflow(i64 a, i64 b, i64& out) { return x86_asm_add_no_overflow_signed(a, b, out); }

bool x86_asm_add_no_overflow(u8 a, u8 b, u8& out) { return x86_asm_add_no_overflow_unsigned(a, b, out); }

bool x86_asm_add_no_overflow(u16 a, u16 b, u16& out) { return x86_asm_add_no_overflow_unsigned(a, b, out); }

bool x86_asm_add_no_overflow(u32 a, u32 b, u32& out) { return x86_asm_add_no_overflow_unsigned(a, b, out); }

bool x86_asm_add_no_overflow(u64 a, u64 b, u64& out) { return x86_asm_add_no_overflow_unsigned(a, b, out); }

#pragma endregion

#pragma region SUB

namespace {

template <typename T>
bool x86_asm_sub_no_overflow_signed(T a, T b, T& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "sub %1, %3\n\t"
        "mov %2, %1\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a), "=m"(out)
        : "r"(b)
    );
    return overflow;
}

template <typename T>
bool x86_asm_sub_no_overflow_unsigned(T a, T b, T& out) {
    bool carry;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "sub %1, %3\n\t"
        "mov %2, %1\n\t"
        "setnc %0\n\t"
        : "=r"(carry), "+r"(a), "=m"(out)
        : "r"(b)
    );
    return carry;
}

} // namespace

bool x86_asm_sub_no_overflow(char a, char b, char& out) { return x86_asm_sub_no_overflow_signed(a, b, out); }
bool x86_asm_sub_no_overflow(i8 a, i8 b, i8& out) { return x86_asm_sub_no_overflow_signed(a, b, out); }
bool x86_asm_sub_no_overflow(i16 a, i16 b, i16& out) { return x86_asm_sub_no_overflow_signed(a, b, out); }
bool x86_asm_sub_no_overflow(i32 a, i32 b, i32& out) { return x86_asm_sub_no_overflow_signed(a, b, out); }
bool x86_asm_sub_no_overflow(i64 a, i64 b, i64& out) { return x86_asm_sub_no_overflow_signed(a, b, out); }
bool x86_asm_sub_no_overflow(u8 a, u8 b, u8& out) { return x86_asm_sub_no_overflow_unsigned(a, b, out); }
bool x86_asm_sub_no_overflow(u16 a, u16 b, u16& out) { return x86_asm_sub_no_overflow_unsigned(a, b, out); }
bool x86_asm_sub_no_overflow(u32 a, u32 b, u32& out) { return x86_asm_sub_no_overflow_unsigned(a, b, out); }
bool x86_asm_sub_no_overflow(u64 a, u64 b, u64& out) { return x86_asm_sub_no_overflow_unsigned(a, b, out); }

#pragma endregion

#pragma region MUL

// TODO2: The number of unnecessary instructions can be reduced for some integer sizes. Only i16, i32 and i64 are optimal.

namespace  {

template <typename T>
inline bool x86_asm_mul_overflow_signed_16bit_to_64bit(T a, T b, T& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "imul %1, %3 \n\t"
        "mov %2, %1\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "+r"(a), "=m"(out)
        : "r"(b)
    );
    return overflow;
}

}

bool x86_asm_mul_overflow(char a, char b, char& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "movzx eax, %2\n\t"
        "imul %3\n\t"
        "mov %1, al\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "=m"(out)
        : "r"(a), "r"(b)
        : "eax", "edx"
    );
    return overflow;
}

bool x86_asm_mul_overflow(i8 a, i8 b, i8 &out) {
    return x86_asm_mul_overflow(static_cast<char>(a), static_cast<char>(b), reinterpret_cast<char&>(out));
}

bool x86_asm_mul_overflow(i16 a, i16 b, i16 &out) { return x86_asm_mul_overflow_signed_16bit_to_64bit(a, b, out); }
bool x86_asm_mul_overflow(i32 a, i32 b, i32 &out) { return x86_asm_mul_overflow_signed_16bit_to_64bit(a, b, out); }
bool x86_asm_mul_overflow(i64 a, i64 b, i64 &out) { return x86_asm_mul_overflow_signed_16bit_to_64bit(a, b, out); }

bool x86_asm_mul_overflow(u8 a, u8 b, u8 &out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "movzx eax, %2\n\t"
        "mul %3\n\t"
        "mov %1, al\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "=m"(out)
        : "r"(a), "r"(b)
        : "eax", "edx"
    );
    return overflow;
}

bool x86_asm_mul_overflow(u16 a, u16 b, u16& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "movzx eax, %2\n\t"
        "mul %3\n\t"
        "mov %1, ax\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "=m"(out)
        : "r"(a), "r"(b)
        : "eax", "edx"
    );
    return overflow;
}

bool x86_asm_mul_overflow(u32 a, u32 b, u32& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "mov eax, %2\n\t"
        "mul %3\n\t"
        "mov %1, eax\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "=m"(out)
        : "r"(a), "r"(b)
        : "eax", "edx"
    );
    return overflow;
}

bool x86_asm_mul_overflow(u64 a, u64 b, u64& out) {
    bool overflow;
    asm volatile (
        ".intel_syntax noprefix \n\t"
        "mov rax, %2\n\t"
        "mul %3\n\t"
        "mov %1, rax\n\t"
        "setno %0\n\t"
        : "=r"(overflow), "=m"(out)
        : "r"(a), "r"(b)
        : "rax", "rdx"
    );
    return overflow;
}

#pragma endregion

} // namespace core
