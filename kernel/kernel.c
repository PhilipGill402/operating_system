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
#include "tty/tty.h"
#include "fs/initrd.h"
#include "fs/fs.h"

#define KERNEL_STACK_SIZE  (PAGE_SIZE * 4)
#define KERNEL_STACK_TOP   0xC0800000
#define KERNEL_STACK_BOTTOM (KERNEL_STACK_TOP - KERNEL_STACK_SIZE)

static multiboot_info_t* mbi;

void finish_init();

__attribute__((noreturn))
void switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void)) {
    asm volatile(
        "mov %0, %%esp    \n\t"
        "xor %%ebp, %%ebp \n\t"
        "jmp *%1          \n\t"
        :
        : "r"(new_stack_top), "r"(next)
        : "memory"
    );

    __builtin_unreachable();
}

void kernel_init(uint32_t mbi_phys) {
    terminal_initialize();

    // mapping mbi into virtual memory
    uint32_t mbi_page_phys = mbi_phys & 0xFFFFF000;
    map_boot_page(mbi_page_phys);

    mbi = (multiboot_info_t*) (mbi_phys + 0xC0000000);
    
    // mapping mmap into virtual memory
    uint32_t start = mbi->mmap_addr & 0xFFFFF000;
    uint32_t end = (mbi->mmap_addr + mbi->mmap_length + PAGE_SIZE - 1) & 0xFFFFF000;

    for (uint32_t page = start; page < end; page += PAGE_SIZE) {
        map_boot_page(page);
    }
    
    pmm_init(mbi);
    transition_page_directory();

    // set up new stack
    for (uint32_t addr = KERNEL_STACK_BOTTOM; addr < KERNEL_STACK_TOP; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        map_page(addr, frame, PAGE_WRITE);
    } 
    
    switch_to_new_kernel_stack(KERNEL_STACK_TOP, finish_init);
}

void finish_init() {
    gdt_install();
    idt_install();
    
    pic_remap(0x20, 0x28);
    irq_init_handlers();
    
    pic_clear_mask(0);
    pic_clear_mask(1);
    pit_init(100);
    
    
    
    init_heap();
    
    fs_init(mbi, initrd_init); 
    
    __asm__ __volatile__("sti");

    tty(); 
}

void kernel_main(uint32_t mbi_phys) {
    kernel_init(mbi_phys);

}
