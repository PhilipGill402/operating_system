#ifndef INCLUDE_EXEC_PROCESS_H_
#define INCLUDE_EXEC_PROCESS_H_

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "interrupts/registers.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"

#define PAGE_SIZE 4096
#define KERNEL_STACK_TOP   0xC0800000
#define KERNEL_STACK_SIZE 8192
#define DEFAULT_MAX_TICKS 10

typedef struct {
    uint32_t start;
    uint32_t end;
} mem_range_t;

typedef enum {
    PROC_RUNNING,
    PROC_READY,
    PROC_BLOCKED,
    PROC_TERMINATED
} proc_state_t;

typedef struct {
    uint32_t pid;
    proc_state_t state;
    uint32_t entry;
    uint32_t user_stack_top;
    uint32_t user_stack_bottom;
    uint32_t kernel_stack_top;
    uint32_t kernel_stack_bottom;
    mem_range_t mem_ranges[MAX_SEGMENTS];
    uint8_t num_ranges;
    uint32_t saved_kernel_esp;
    uint32_t saved_kernel_ebp;
    regs_t* trapframe;
    uint32_t ticks_left;
} process_t;

extern process_t* current_process;
extern uint32_t num_processes;

__attribute__((noreturn)) void enter_user_mode_from_trapframe(const regs_t *tf);
void process_init_trapframe(process_t* process);
void process_destroy(process_t* process);

#endif // !INCLUDE_EXEC_PROCESS_H_
