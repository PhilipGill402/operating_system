#include <syscalls.h>

int32_t execve(const char* path, const char* argv[]) { 
    return __sys2(SYS_EXECVE, (uint32_t)path, (uint32_t)argv);
}
