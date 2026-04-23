#include <syscalls.h>

uint32_t getpid() {
    return __sys0(SYS_GETPID);
}
