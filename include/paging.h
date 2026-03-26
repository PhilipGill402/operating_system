#ifndef INCLUDE_PAGING_H_
#define INCLUDE_PAGING_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "physical_allocator.h"

#define PAGE_SIZE 4096
#define KERNEL_BASE 0xC0000000
#define PAGE_PRESENT 0x001
#define PAGE_WRITE 0x002
#define TEMP_PT_VIRT 0xC0400000
#define TEMP_PD_INDEX 769

extern uint32_t* temp_page_table;

extern uint32_t boot_page_directory[1024];
extern uint32_t boot_page_table1[1024];

extern uint32_t* kernel_page_directory;
extern uint32_t kernel_page_directory_phys;

static inline void invlpg(void* addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(addr) : "memory");
}

static inline void load_cr3(uint32_t phys) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(phys) : "memory");
}

void map_boot_page(uint32_t phys_addr);
void transition_page_directory();
void map_page(uint32_t virt, uint32_t phys, uint32_t flags);
#endif // !INCLUDE_PAGING_H_
