#include <syscalls.h>

__attribute__((noreturn))
void exit(int32_t error_code) {
    asm volatile (
            "movl %0, %%eax\n\t"
            "movl %1, %%ebx\n\t"
            "int $0x80\n\t"
            :
            : "r"(SYS_EXIT), "r"(error_code)
            : "eax", "ebx", "memory"
    );

    for (;;) {}
}
