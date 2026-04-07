#include "exec/process.h"

process_t* current_process;

__attribute__((noreturn))
void enter_user_mode(uint32_t entry, uint32_t user_stack_top) {
    __asm__ __volatile__(
        "cli                    \n\t"

        /* load user data selectors into data segment registers */
        "mov %0, %%ax           \n\t"
        "mov %%ax, %%ds         \n\t"
        "mov %%ax, %%es         \n\t"
        "mov %%ax, %%fs         \n\t"
        "mov %%ax, %%gs         \n\t"

        /* build iret frame for ring 3 */
        "pushl %0               \n\t"  /* SS  */
        "pushl %1               \n\t"  /* ESP */
        "pushfl                 \n\t"  /* EFLAGS */
        "pushl %2               \n\t"  /* CS  */
        "pushl %3               \n\t"  /* EIP */

        "iret                   \n\t"
        :
        : "i"(USER_DS_RING3),
          "r"(user_stack_top),
          "i"(USER_CS_RING3),
          "r"(entry)
        : "ax", "memory"
    );

    __builtin_unreachable();
}

void process_destroy(process_t* process) {
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

}
