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
#define PAGE_USER 0x004

#define TEMP_PT_VIRT_0 0xC0400000
#define TEMP_PD_INDEX_0 769
#define TEMP_PT_VIRT_1 0xC0800000
#define TEMP_PD_INDEX_1 770
#define TEMP_PT_VIRT_2 0xC0C00000
#define TEMP_PD_INDEX_2 771
#define TEMP_PT_VIRT_3 0xC1000000
#define TEMP_PD_INDEX_3 772
#define TEMP_PT_VIRT_4 0xC1400000
#define TEMP_PD_INDEX_4 773
#define TEMP_PT_VIRT_5 0xC1800000
#define TEMP_PD_INDEX_5 774

#define KERNEL_VIRT_START 0xC2000000
#define KERNEL_VIRT_END   0xE0000000

extern uint32_t next_free_virt; 

extern uint32_t* temp_page_table1;

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
void paging_init_temp_regions();
void transition_page_directory();
void map_page(uint32_t virt, uint32_t phys, uint32_t flags);
void map_user_page(uint32_t* page_directory, uint32_t virt, uint32_t phys, uint32_t flags);
uint32_t unmap_page(uint32_t virt);
void* alloc_kernel_page(uint32_t flags);
void free_kernel_page(uint32_t virt);
uint32_t* temp_map_phys0(uint32_t phys);
uint32_t* temp_map_phys1(uint32_t phys);
uint32_t* temp_map_phys2(uint32_t phys);
uint32_t* temp_map_phys3(uint32_t phys);
uint32_t* temp_map_phys4(uint32_t phys);
#endif // !INCLUDE_PAGING_H_
