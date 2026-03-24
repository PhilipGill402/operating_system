#include "paging.h"

void map_boot_page(uint32_t phys_addr) {
    uint32_t virt_addr = phys_addr + KERNEL_BASE;
    uint32_t pt_idx = (virt_addr >> 12) & 0x3FF;

    boot_page_table1[pt_idx] = (phys_addr & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;
    invlpg((void*)virt_addr);
}
