#include <syscalls.h>

int32_t write(uint32_t fd, const char* buffer, size_t count) {
    int32_t ret; 
    asm volatile(
    "int $0x80"
    : "=a"(ret)
    : "a"(SYS_WRITE), "b"(fd), "c"(buffer), "d"(count)
    : "memory"
    );

    return ret;
}
