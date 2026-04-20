#include <syscalls.h>

int32_t chdir(const char* path) {
    return __sys1(SYS_CHDIR, (uint32_t)path);
}
