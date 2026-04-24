#include <syscalls.h>

void* sbrk(size_t increment) {
    void* old_break = brk(0);

    uint32_t new_addr = (uint32_t)old_break + increment; // get current heap break
    
    void* new_base = brk((void*)new_addr);
    if (!new_base) return NULL;

    return old_break;
}
