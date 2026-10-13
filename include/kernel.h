#ifndef INCLUDE_KERNEL_H_
#define INCLUDE_KERNEL_H_

#include <stdint.h>
#include <stddef.h>

#define MAX_MEMORY_REGIONS  64

typedef enum memory_type {
    MEM_USABLE,
    MEM_RESERVED
} memory_type_t;

typedef struct memory_region {
    uintptr_t mem_addr;
    size_t mem_size;
    memory_type_t type;
} memory_region_t;

typedef struct kernel_boot_info {
    memory_region_t regions[MAX_MEMORY_REGIONS];
    size_t region_count;

    uint64_t timer_frequency;
} kernel_boot_info_t;

void kernel_main(const kernel_boot_info_t* info);

#endif
