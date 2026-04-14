#include "syscalls.h"

void sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();

    for (;;) {}
}

void sys_write(const char* str) {
    printf("%s", str);
}

void syscall_handler(regs_t* reg) {
    switch (reg->eax) {
        case SYS_EXIT: 
            sys_exit(reg);
            break;
        case SYS_WRITE:
            sys_write(reg->ebx);
            break;
    }
}
