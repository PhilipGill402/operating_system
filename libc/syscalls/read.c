#include <syscalls.h>

int32_t read(uint32_t fd, char* buffer, size_t count) {
    int32_t ret; 
    asm volatile(
    "int $0x80"
    : "=a"(ret)
    : "a"(SYS_READ), "b"(fd), "c"(buffer), "d"(count)
    : "memory"
    );

    return ret;
}
