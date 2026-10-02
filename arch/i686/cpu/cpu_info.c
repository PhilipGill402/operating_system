#include <arch/cpu/cpu_info.h>

#include <string.h>
#include "memory/heap.h"

static inline void cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(leaf)
    );
}

char* cpu_get_vendor() {
    char* vendor = kmalloc(13);
    
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, &eax, &ebx, &ecx, &edx);

    memcpy(vendor, &ebx, 4);
    memcpy(vendor + 4, &edx, 4);
    memcpy(vendor + 8, &ecx, 8);
    vendor[12] = '\0';
    
    return vendor;
}

char* cpu_get_brand() {
    char* brand = kmalloc(49);

    uint32_t eax, ebx, ecx, edx;
    uint32_t max_ext;

    cpuid(0x80000000, &max_ext, &ebx, &ecx, &edx);

    if (max_ext < 0x80000004) {
        strcpy(brand, "Unknown CPU");
        return brand;
    }

    uint32_t* out = (uint32_t*)brand;

    cpuid(0x80000002, &out[0],  &out[1],  &out[2],  &out[3]);
    cpuid(0x80000003, &out[4],  &out[5],  &out[6],  &out[7]);
    cpuid(0x80000004, &out[8],  &out[9],  &out[10], &out[11]);

    brand[48] = '\0';

    return brand;
}
