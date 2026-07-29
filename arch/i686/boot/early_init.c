#include <arch/boot.h>
#include <log.h>

#include "io/serial.h"
#include "multiboot.h"
#include "memory_mapping.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "gdt.h"


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

void arch_kernel_early_init(uint32_t mbi_phys, void (*kernel_finish_init)(void)) {
    serial_init(); 

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
    paging_init_temp_regions();
    transition_page_directory();
    
    // set up new stack
    for (uint32_t addr = KERNEL_STACK_BOTTOM; addr < KERNEL_STACK_TOP; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            log_error("failed to allocate frame");
        }

        map_page(addr, frame, PAGE_WRITE);
    }

    arch_switch_to_new_kernel_stack(KERNEL_STACK_TOP, kernel_finish_init);
}
