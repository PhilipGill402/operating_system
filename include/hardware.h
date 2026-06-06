#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <stdint.h>

#include <string.h>

#include "memory/heap.h"

static inline void cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(leaf)
    );
}

char* cpu_get_vendor();
char* cpu_get_brand();

#endif
