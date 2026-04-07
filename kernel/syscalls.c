#include "syscalls.h"

void sys_exit() {
    process_destroy(current_process); 

    current_process = NULL;
    
    __asm__ __volatile__("sti");

    tty();
}

void sys_write(const char* str) {
    printf("%s", str);
}

void syscall_handler(regs_t* reg) {
    if (reg->eax == SYS_EXIT) {
        sys_exit();
    } else if (reg->eax == SYS_WRITE) {
        sys_write(reg->ebx);
    }
}
