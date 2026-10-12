#ifndef INCLUDE_KERNEL_H_
#define INCLUDE_KERNEL_H_

#include <stdint.h>
#include <stddef.h>

typedef struct kernel_boot_info {
    uintptr_t memory_base;
    size_t memory_size;

    uintptr_t boot_info;
} kernel_boot_info_t;

void kernel_main(const kernel_boot_info_t* info);

#endif
