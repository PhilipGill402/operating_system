#include <syscalls.h>

int32_t write(uint32_t fd, const char* buffer, size_t count) {
    return __sys3(SYS_WRITE, fd, (uint32_t)buffer, (uint32_t)count);
}
