bits 32
section .text._start
global _start
extern kmain

_start:
    mov dword [0xB8000], 0x0F310F4B   ; 'K''1' (bright white)
    call kmain
.hang:
    hlt
    jmp .hang

