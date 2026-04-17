.global _start
.extern main
.extern exit

_start:
    call main
    push %eax
    call exit

1:
    jmp 1b
