#ifndef INCLUDE_PHYSICAL_ALLOCATOR_H_
#define INCLUDE_PHYSICAL_ALLOCATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <arch/memory/memory_map.h>
#include "kernel.h"



typedef struct {
    uint32_t* bitmap;          // 1 bit per 4 KiB frame
    uintptr_t phys_base; 
    uint32_t total_frames;
    uint32_t used_frames;
    uint32_t free_frames;
    uint32_t search_hint;      // where to resume scanning
} pmm_t;

extern pmm_t pmm;

void pmm_init(const kernel_boot_info_t* info);
uintptr_t pmm_alloc_frame(void);          // returns physical address
void pmm_free_frame(uintptr_t phys_addr);

#endif // !INCLUDE_PHYSICAL_ALLOCATOR_H_

