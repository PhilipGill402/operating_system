#ifndef INCLUDE_PAGING_H_
#define INCLUDE_PAGING_H_

#include <stdint.h>

#define PAGE_SIZE 4096
#define KERNEL_BASE 0xC0000000
#define PAGE_PRESENT 0x001
#define PAGE_WRITE 0x002

extern uint32_t boot_page_directory[1024];
extern uint32_t boot_page_table1[1024];

static inline void invlpg(void* addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(addr) : "memory");
}

void map_boot_page(uint32_t phys_addr);

#endif // !INCLUDE_PAGING_H_
