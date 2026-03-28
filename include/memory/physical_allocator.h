#ifndef INCLUDE_PHYSICAL_ALLOCATOR_H_
#define INCLUDE_PHYSICAL_ALLOCATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "multiboot.h"
#include "paging.h"

#define PAGE_SIZE 4096
#define KERNEL_BASE 0xC0000000
#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))

extern char _kernel_start;
extern char _kernel_end;

extern uint32_t boot_page_directory[1024];
extern uint32_t boot_page_table1[1024];

typedef struct {
    uint32_t* bitmap;          // 1 bit per 4 KiB frame
    uint32_t bitmap_bytes;
    uint32_t total_frames;
    uint32_t used_frames;
    uint32_t free_frames;
    uint32_t search_hint;      // where to resume scanning
} pmm_t;

void pmm_init(multiboot_info_t* mbi);
uint32_t pmm_alloc_frame(void);          // returns physical address
void pmm_free_frame(uint32_t phys_addr);

#endif // !INCLUDE_PHYSICAL_ALLOCATOR_H_

