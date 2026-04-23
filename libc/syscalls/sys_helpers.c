#include <syscalls.h>

uint32_t __sys0(uint32_t sys_num) {
    uint32_t ret; 
    asm volatile (
            "movl %1, %%eax\n\t"
            "int $0x80\n\t"
            "movl %%eax, %0\n\t"
            : "=r"(ret)
            : "r"(sys_num)
            : "eax", "memory"
    );

    return ret;
}

uint32_t __sys1(uint32_t sys_num, uint32_t arg1) {
    uint32_t ret; 
    asm volatile (
            "movl %1, %%eax\n\t"
            "movl %2, %%ebx\n\t"
            "int $0x80\n\t"
            : "=r"(ret)
            : "r"(sys_num), "r"(arg1)
            : "eax", "ebx", "memory"
    );

    return ret;
}

uint32_t __sys2(uint32_t sys_num, uint32_t arg1, uint32_t arg2) {
    uint32_t ret; 
    asm volatile (
            "movl %1, %%eax\n\t"
            "movl %2, %%ebx\n\t"
            "movl %3, %%ecx\n\t"
            "int $0x80\n\t"
            "movl %%eax, %0\n\t"
            : "=r"(ret)
            : "r"(sys_num), "r"(arg1), "r"(arg2)
            : "eax", "ebx", "ecx", "memory"
    );

    return ret;
}

uint32_t __sys3(uint32_t sys_num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    uint32_t ret; 
    asm volatile(
    "int $0x80"
    : "=a"(ret)
    : "a"(sys_num), "b"(arg1), "c"(arg2), "d"(arg3)
    : "memory"
    );

    return ret;
}
