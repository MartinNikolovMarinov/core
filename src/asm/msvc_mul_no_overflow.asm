OPTION CASEMAP:NONE

PUBLIC x86_asm_mul_no_overflow_char
PUBLIC x86_asm_mul_no_overflow_i16
PUBLIC x86_asm_mul_no_overflow_i32
PUBLIC x86_asm_mul_no_overflow_i64
PUBLIC x86_asm_mul_no_overflow_u8
PUBLIC x86_asm_mul_no_overflow_u16
PUBLIC x86_asm_mul_no_overflow_u32
PUBLIC x86_asm_mul_no_overflow_u64

.code

; bool x86_asm_mul_no_overflow_char(char a, char b, char* out);
x86_asm_mul_no_overflow_char PROC
    mov al, cl         ; Move first operand into AL
    imul dl            ; Multiply AL (a) with DL (b), result in AX
    mov [r8], al       ; Store result in out (low byte of AX)
    setno dl           ; Set dl = 1 if no signed overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_char ENDP

; bool x86_asm_mul_no_overflow_i16(i16 a, i16 b, i16* out);
x86_asm_mul_no_overflow_i16 PROC
    mov ax, cx         ; Move first operand into AX
    imul dx            ; Multiply AX (a) with DX (b), result in DX:AX
    mov word ptr [r8], ax ; Store lower 16 bits of result in out
    setno dl           ; Set dl = 1 if no signed overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_i16 ENDP

; bool x86_asm_mul_no_overflow_i32(i32 a, i32 b, i32* out);
x86_asm_mul_no_overflow_i32 PROC
    imul ecx, edx      ; Perform signed 32-bit multiplication
    mov dword ptr [r8], ecx ; Store result in out
    setno dl           ; Set dl = 1 if no signed overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_i32 ENDP

; bool x86_asm_mul_no_overflow_i64(i64 a, i64 b, i64* out);
x86_asm_mul_no_overflow_i64 PROC
    imul rcx, rdx      ; Perform signed 64-bit multiplication
    mov qword ptr [r8], rcx ; Store result in out
    setno dl           ; Set dl = 1 if no signed overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_i64 ENDP

; bool x86_asm_mul_no_overflow_u8(u8 a, u8 b, u8* out);
x86_asm_mul_no_overflow_u8 PROC
    mov al, cl         ; Move first operand into AL
    mul dl             ; Unsigned multiply AL with DL, result in AX
    mov [r8], al       ; Store result in out (low byte of AX)
    setnc dl           ; Set dl = 1 if no unsigned overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_u8 ENDP

; bool x86_asm_mul_no_overflow_u16(u16 a, u16 b, u16* out);
x86_asm_mul_no_overflow_u16 PROC
    mov ax, cx         ; Move first operand into AX
    mul dx             ; Unsigned multiply AX with DX, result in DX:AX
    mov word ptr [r8], ax ; Store lower 16 bits of result in out
    setnc dl           ; Set dl = 1 if no unsigned overflow occurred
    movzx eax, dl      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_u16 ENDP

; bool x86_asm_mul_no_overflow_u32(u32 a, u32 b, u32* out);
x86_asm_mul_no_overflow_u32 PROC
    mov eax, ecx       ; Move first operand (a) into EAX
    mul edx            ; Unsigned multiply EAX (a) with EDX (b), result in EDX:EAX
    mov dword ptr [r8], eax ; Store lower 32 bits of result into out
    test edx, edx      ; Check if upper 32 bits (EDX) are zero
    setz al            ; AL = 1 if no overflow (EDX == 0), 0 otherwise
    movzx eax, al      ; Return result as a boolean in EAX
    ret
x86_asm_mul_no_overflow_u32 ENDP

; bool x86_asm_mul_no_overflow_u64(u64 a, u64 b, u64* out);
x86_asm_mul_no_overflow_u64 PROC
    mov rax, rcx       ; Move first operand into RAX
    mul rdx            ; Unsigned multiply RAX (a) with RDX (b), result in RDX:RAX
    mov [r8], rax      ; Store lower 64 bits in out
    test rdx, rdx      ; Check if upper 64 bits (RDX) are zero
    setz al            ; AL = 1 if no overflow occurred
    movzx eax, al      ; Return result as boolean
    ret
x86_asm_mul_no_overflow_u64 ENDP

END
