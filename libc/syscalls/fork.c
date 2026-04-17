#include <syscalls.h>

int32_t fork() {
    int32_t ret; 
    asm volatile (
            "movl %1, %%eax\n\t"
            "int $0x80\n\t"
            "movl %%eax, %0\n\t"
            : "=r"(ret)
            : "r"(SYS_FORK)
            : "eax", "memory"
    );
}
