#include "syscalls.h"

void sys_exit() {
    // cleans up all the mapped virtual memory
    for (uint8_t i = 0; i < current_process->num_ranges; i++) {
        uint32_t start = current_process->mem_ranges[i].start;
        uint32_t end = current_process->mem_ranges[i].end;

        for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
            uint32_t frame = unmap_page(addr);
            if (frame) {
                pmm_free_frame(frame);
            }
        }
    }

    // cleans up the stack memory
    for (uint32_t addr = current_process->user_stack_bottom; addr < current_process->user_stack_top; addr += PAGE_SIZE) {
        uint32_t frame = unmap_page(addr);
        if (frame) {
            pmm_free_frame(frame);
        }
    }

    current_process = NULL;
    
    __asm__ __volatile__("sti");

    tty();
}

void syscall_handler(regs_t* reg) {
    if (reg->eax == SYS_EXIT) {
        sys_exit();
    }
}
