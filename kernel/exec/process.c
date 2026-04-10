#include "exec/process.h"

process_t* current_process = NULL;
uint32_t num_processes = 0;

void process_init_trapframe(process_t* process) {
    regs_t* tf = (regs_t*)(process->kernel_stack_top - sizeof(regs_t));
    memset(tf, 0, sizeof(regs_t));

    tf->eip = process->entry;
    tf->cs = USER_CS_RING3;
    tf->eflags = 0x202;
    tf->useresp = process->user_stack_top;
    tf->ss = USER_DS_RING3;
    tf->ds = USER_DS_RING3;

    process->trapframe = tf;
    process->saved_kernel_esp = (uint32_t)tf;
    process->saved_kernel_ebp = 0;

}

void process_destroy(process_t* process) {
    // cleans up all the mapped virtual memory
    for (uint8_t i = 0; i < process->num_ranges; i++) {
        uint32_t start = process->mem_ranges[i].start;
        uint32_t end = process->mem_ranges[i].end;

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

}
