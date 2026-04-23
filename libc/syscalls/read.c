#include <syscalls.h>

uint32_t read(uint32_t fd, char* buffer, size_t count) {
    return __sys3(SYS_READ, fd, (uint32_t)buffer, (uint32_t)count);
}
