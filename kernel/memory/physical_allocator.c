#include "memory/physical_allocator.h"
#include "kernel.h"
#include <string.h>
#include <helpers.h>

#include <log.h>

pmm_t pmm;

static inline void bitmap_set(uintptr_t frame) {
    uint64_t idx = frame / 32;
    uint64_t bit = frame % 32;
    uint64_t mask = 1u << bit;

    if (!(pmm.bitmap[idx] & mask)) {
        pmm.bitmap[idx] |= mask;
        pmm.used_frames++;
        pmm.free_frames--;
    }
}

static inline void bitmap_clear(uintptr_t frame) {
    uint64_t idx = frame / 32;
    uint64_t bit = frame % 32;
    uint64_t mask = 1u << bit;

    if (pmm.bitmap[idx] & mask) {
        pmm.bitmap[idx] &= ~mask;
        pmm.used_frames--;
        pmm.free_frames++;
    } 
}

static inline bool bitmap_test(uintptr_t frame) {
    return (pmm.bitmap[frame / 32] >> (frame % 32)) & 1;
}

static void pmm_reserve_phys_range(uintptr_t phys_start, size_t size) {
    uintptr_t page_start = ALIGN_DOWN(phys_start, PAGE_SIZE);
    uintptr_t page_end = ALIGN_UP(phys_start + size, PAGE_SIZE);

    for (uintptr_t page = page_start; page < page_end; page += PAGE_SIZE) {
        bitmap_set((page - pmm.phys_base) / PAGE_SIZE);
    }
}

static void pmm_free_phys_range(uintptr_t phys_start, size_t size) {
    uintptr_t page_start = ALIGN_DOWN(phys_start, PAGE_SIZE);
    uintptr_t page_end = ALIGN_UP(phys_start + size, PAGE_SIZE);

    for (uintptr_t page = page_start; page < page_end; page += PAGE_SIZE) {
        bitmap_clear((page - pmm.phys_base) / PAGE_SIZE);
    }
}

void pmm_init(const kernel_boot_info_t* info) {
    // find lowest and highest physical address
    uintptr_t lowest_addr = 0;
    uintptr_t highest_addr = 0;
    for (size_t i = 0; i < info->region_count; ++i) {
        if (info->regions[i].mem_addr < lowest_addr || lowest_addr == 0)
            lowest_addr = info->regions[i].mem_addr;

        if (info->regions[i].mem_addr + info->regions[i].mem_size > highest_addr)
            highest_addr = info->regions[i].mem_addr + info->regions[i].mem_size;
    }
    
    lowest_addr = ALIGN_DOWN(lowest_addr, PAGE_SIZE);
    highest_addr = ALIGN_UP(highest_addr, PAGE_SIZE);
    
    uint64_t page_count = (highest_addr - lowest_addr) / PAGE_SIZE;
    
    // place bitmap right after kernel
    pmm.bitmap = (uint32_t*)ALIGN_UP((uintptr_t)kernel_end, PAGE_SIZE);
    pmm.phys_base = lowest_addr;
    pmm.total_frames = page_count;
    pmm.used_frames = page_count;
    pmm.free_frames = 0;
    pmm.search_hint = 0;

    uint64_t bitmap_size = DIV_CEIL(pmm.total_frames, 8);
    memset(pmm.bitmap, 0xFFFFFFFF, bitmap_size);
    
    // set all usable memory pages as free
    for (size_t i = 0; i < info->region_count; ++i) {
        if (info->regions[i].type == MEM_USABLE) {
            pmm_free_phys_range(info->regions[i].mem_addr, info->regions[i].mem_size);
        }
            
    }

    // re-reserve any reserved pages just incase they were freed by the previous loop
    for (size_t i = 0; i < info->region_count; ++i) {
        if (info->regions[i].type == MEM_RESERVED || info->regions[i].type == MEM_FILESYSTEM)
            pmm_reserve_phys_range(info->regions[i].mem_addr, info->regions[i].mem_size);
    }
    
    // reserve all kernel used pages
    pmm_reserve_phys_range((uintptr_t)kernel_start, (uintptr_t)kernel_end - (uintptr_t)kernel_start);   // reserve kernel
    pmm_reserve_phys_range((uintptr_t)pmm.bitmap, bitmap_size);                                         // reserve bitmap
}

uintptr_t pmm_alloc_frame(void)  {          // returns physical address
    for (uintptr_t frame = pmm.search_hint, total = 0; total < pmm.total_frames; frame = (frame + 1) % pmm.total_frames, total++) {
        // frame is free
        if (!bitmap_test(frame)) {
            bitmap_set(frame);
            pmm.search_hint = (frame + 1) % pmm.total_frames;
            return pmm.phys_base + (frame * PAGE_SIZE);
        } 
    }

    return 0;
}

void pmm_free_frame(uintptr_t phys_addr) {
    // phys_addr out of range
    if (phys_addr < pmm.phys_base || phys_addr >= pmm.phys_base + (pmm.total_frames * PAGE_SIZE))
        return;

    uintptr_t frame = (ALIGN_DOWN(phys_addr, PAGE_SIZE) - pmm.phys_base) / PAGE_SIZE;
    bitmap_clear(frame);
}


