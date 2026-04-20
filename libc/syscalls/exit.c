#include <syscalls.h>

__attribute__((noreturn))
void exit(int32_t error_code) {
    __sys1(SYS_EXIT, error_code); 

    for (;;) {}
}
