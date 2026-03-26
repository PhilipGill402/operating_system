#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include "vga.h"
#include "idt.h"
#include "gdt.h"
#include "multiboot.h"
#include "physical_allocator.h"
#include "paging.h"

void kernel_main(unsigned long mbi_phys) {
    terminal_initialize();

    // mapping mbi into virtual memory
    uint32_t mbi_page_phys = mbi_phys & 0xFFFFF000;
    map_boot_page(mbi_page_phys);

    multiboot_info_t* mbi = (multiboot_info_t*) (mbi_phys + 0xC0000000);
    
    // mapping mmap into virtual memory
    uint32_t start = mbi->mmap_addr & 0xFFFFF000;
    uint32_t end = (mbi->mmap_addr + mbi->mmap_length + PAGE_SIZE - 1) & 0xFFFFF000;

    for (uint32_t page = start; page < end; page += PAGE_SIZE) {
        map_boot_page(page);
    }

    gdt_install();
    idt_install();
    pmm_init(mbi);
    transition_page_directory();

    uint32_t phys = pmm_alloc_frame();
    uint32_t virt = 0xC0500000;
    map_page(virt, phys, PAGE_WRITE);


    printf("Hello world\n");
}
