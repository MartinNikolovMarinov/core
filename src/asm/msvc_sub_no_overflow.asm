OPTION CASEMAP:NONE

PUBLIC x86_asm_sub_no_overflow_char
PUBLIC x86_asm_sub_no_overflow_i16
PUBLIC x86_asm_sub_no_overflow_i32
PUBLIC x86_asm_sub_no_overflow_i64
PUBLIC x86_asm_sub_no_overflow_u8
PUBLIC x86_asm_sub_no_overflow_u16
PUBLIC x86_asm_sub_no_overflow_u32
PUBLIC x86_asm_sub_no_overflow_u64

.code

; bool x86_asm_sub_no_overflow_char(char a, char b, char* out);
; a in RCX (cl), b in RDX (dl), out in R8
x86_asm_sub_no_overflow_char PROC
    sub cl, dl
    mov [r8], cl
    setno dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_char ENDP

; bool x86_asm_sub_no_overflow_i16(i16 a, i16 b, i16* out);
; a in cx, b in dx, out in r8
x86_asm_sub_no_overflow_i16 PROC
    sub cx, dx
    mov word ptr [r8], cx
    setno dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_i16 ENDP

; bool x86_asm_sub_no_overflow_i32(i32 a, i32 b, i32* out);
x86_asm_sub_no_overflow_i32 PROC
    sub ecx, edx
    mov dword ptr [r8], ecx
    setno dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_i32 ENDP

; bool x86_asm_sub_no_overflow_i64(i64 a, i64 b, i64* out);
x86_asm_sub_no_overflow_i64 PROC
    sub rcx, rdx
    mov qword ptr [r8], rcx
    setno dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_i64 ENDP

; bool x86_asm_sub_no_overflow_u8(u8 a, u8 b, u8* out);
x86_asm_sub_no_overflow_u8 PROC
    sub cl, dl
    mov [r8], cl
    setnc dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_u8 ENDP

; bool x86_asm_sub_no_overflow_u16(u16 a, u16 b, u16* out);
x86_asm_sub_no_overflow_u16 PROC
    sub cx, dx
    mov word ptr [r8], cx
    setnc dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_u16 ENDP

; bool x86_asm_sub_no_overflow_u32(u32 a, u32 b, u32* out);
x86_asm_sub_no_overflow_u32 PROC
    sub ecx, edx
    mov dword ptr [r8], ecx
    setnc dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_u32 ENDP

; bool x86_asm_sub_no_overflow_u64(u64 a, u64 b, u64* out);
x86_asm_sub_no_overflow_u64 PROC
    sub rcx, rdx
    mov qword ptr [r8], rcx
    setnc dl
    movzx eax, dl
    ret
x86_asm_sub_no_overflow_u64 ENDP

END
