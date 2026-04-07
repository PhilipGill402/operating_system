#ifndef INCLUDE_EXEC_PROCESS_H_
#define INCLUDE_EXEC_PROCESS_H_

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

#include <stdint.h>

#define PAGE_SIZE 4096

typedef struct {
    uint32_t start;
    uint32_t end;
} mem_range_t;

typedef struct {
    uint32_t entry;
    uint32_t user_stack_top;
    uint32_t user_stack_bottom;
    mem_range_t mem_ranges[MAX_SEGMENTS];
    uint8_t num_ranges;
    uint32_t saved_kernel_esp;
    uint32_t saved_kernel_ebp;
} process_t;

extern process_t* current_process;

__attribute__((noreturn))
void enter_user_mode(uint32_t entry, uint32_t user_stack_top);
void process_destroy(process_t* process);

#endif // !INCLUDE_EXEC_PROCESS_H_
