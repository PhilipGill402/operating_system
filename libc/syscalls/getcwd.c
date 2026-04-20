#include <syscalls.h>

int32_t getcwd(char* buffer, size_t size) {
    return __sys2(SYS_GETCWD, (uint32_t)buffer, size);
}
