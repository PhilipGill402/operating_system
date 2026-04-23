#include <syscalls.h>

uint32_t chdir(const char* path) {
    return __sys1(SYS_CHDIR, (uint32_t)path);
}
