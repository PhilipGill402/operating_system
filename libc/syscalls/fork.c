#include <syscalls.h>

uint32_t fork() {
    return __sys0(SYS_FORK); 
}
