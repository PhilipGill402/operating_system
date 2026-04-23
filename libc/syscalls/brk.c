#include <syscalls.h>

void* brk(void* new_addr) {
    void* ret = (void*)__sys1(SYS_BRK, (uint32_t)new_addr);
    
    return ret;
}
