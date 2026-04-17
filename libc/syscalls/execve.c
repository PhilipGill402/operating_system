#include <syscalls.h>

int32_t execve(const char* path, const char* argv[]) {
    int32_t ret; 
    asm volatile (
            "movl %1, %%eax\n\t"
            "movl %2, %%ebx\n\t"
            "movl %3, %%ecx\n\t"
            "int $0x80\n\t"
            "movl %%eax, %0\n\t"
            : "=r"(ret)
            : "r"(SYS_EXECVE), "r"(path), "r"(argv)
            : "eax", "ebx", "ecx", "memory"
    );

    return ret;
}
