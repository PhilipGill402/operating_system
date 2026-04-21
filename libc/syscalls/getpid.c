#include <syscalls.h>

int32_t getpid() {
    return __sys0(SYS_GETPID);
}
