#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "vga.h"
#include "gdt.h"
#include "multiboot.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"

void kernel_main(uint32_t mbi_phys) {
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
    
    pic_remap(0x20, 0x28);
    irq_init_handlers();
    
    pic_clear_mask(0);
    pic_clear_mask(1);
    pit_init(100);

    pmm_init(mbi);
    transition_page_directory();

    __asm__ __volatile__("sti");
    
    printf("Hello world\n");

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
