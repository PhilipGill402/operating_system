#include <arch/boot/boot.h>
#include <log.h>

#include <arch/memory/physical_allocator.h>
#include <arch/cpu/gdt.h>
#include <arch/asm/helpers.h>
#include <arch/memory/paging.h>
#include <arch/interrupts/idt.h>
#include <arch/interrupts/pic.h>
#include <arch/interrupts/irq.h>
#include <arch/interrupts/pit.h>

#include <memory/internal_paging.h>

#include "io/serial.h"
#include "multiboot.h"
#include "memory_mapping.h"

extern void kernel_finish_init();

__attribute__((noreturn))
void arch_switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void)) {
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

void arch_kernel_init(void) {
    // setup i686 specific internals
    gdt_install();
    idt_install();
    
    pic_remap(0x20, 0x28);
    
    irq_init_handlers();
    
    pic_clear_mask(0);
    pic_clear_mask(1);
    pit_init(100);

    kernel_finish_init();
}

void arch_kernel_early_init(uint32_t mbi_phys) {
    serial_init(); 

    // mapping mbi into virtual memory
    uint32_t mbi_page_phys = mbi_phys & 0xFFFFF000;
    i686_map_boot_page(mbi_page_phys);
    mbi = (multiboot_info_t*) (mbi_phys + 0xC0000000);
    
    // mapping mmap into virtual memory
    uint32_t start = mbi->mmap_addr & 0xFFFFF000;
    uint32_t end = (mbi->mmap_addr + mbi->mmap_length + PAGE_SIZE - 1) & 0xFFFFF000;
    
    for (uint32_t page = start; page < end; page += PAGE_SIZE) {
        i686_map_boot_page(page);
    }

    // Paging setup
    pmm_init(mbi);
    arch_paging_transition();
    i686_create_temp_page_table();
    i686_init_shared_region();
    
    // set up new stack
    for (uint32_t addr = KERNEL_STACK_BOTTOM; addr < KERNEL_STACK_TOP; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            log_error("failed to allocate frame");
            arch_halt();
        }

        arch_page_map(arch_kernel_address_space(), addr, frame, ARCH_PAGE_WRITE);
    }

    arch_switch_to_new_kernel_stack(KERNEL_STACK_TOP, arch_kernel_init);
}



