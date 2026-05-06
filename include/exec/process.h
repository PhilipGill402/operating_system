#ifndef INCLUDE_EXEC_PROCESS_H_
#define INCLUDE_EXEC_PROCESS_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "interrupts/registers.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "fs/fs_types.h"

#define MAX_FDS 128

#define KERNEL_BASE 0xC0000000

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

#define PAGE_SIZE 4096
#define KERNEL_STACK_SIZE  (PAGE_SIZE * 4)
#define KERNEL_STACK_BOTTOM 0xC0C00000
#define KERNEL_STACK_TOP (KERNEL_STACK_BOTTOM + KERNEL_STACK_SIZE)
#define DEFAULT_MAX_TICKS 10

#define USER_NULL_GUARD      0x00000000
#define USER_STACK_TOP       0xBFFFE000
#define USER_STACK_SIZE      (PAGE_SIZE * 4)
#define USER_STACK_BOTTOM    (USER_STACK_TOP - USER_STACK_SIZE)

#define MAX_PROCESSES 64

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
    char name[128]; 

    uint32_t pid;
    uint32_t ppid; 
    proc_state_t state;
    uint32_t entry;

    int32_t exit_status;
    uint8_t waited_on;
    uint32_t waiting_for_pid;
    int32_t wait_result_status; 
    int32_t* waiting_status_ptr;
    uint8_t wait_has_results;

    uint32_t image_base;
    uint32_t image_end;
    
    uint32_t heap_start;
    uint32_t heap_break;
    uint32_t heap_mapped_end;
    uint32_t heap_max_end;

    uint32_t user_stack_top;
    uint32_t user_stack_bottom;

    uint32_t kernel_stack_top;
    uint32_t kernel_stack_bottom;
    uint32_t saved_kernel_esp;
    uint32_t saved_kernel_ebp;
    
    uint32_t page_directory_phys;
    mem_range_t mem_ranges[MAX_SEGMENTS];
    uint8_t num_ranges;
    
    regs_t* trapframe;
    uint32_t ticks_left;
    file_desc_t fds[MAX_FDS];
    uint32_t open_fds;
    fs_node_t* cwd;
} process_t;

extern process_t* current_process;
extern process_t* process_table[MAX_PROCESSES];
extern uint32_t num_processes;

__attribute__((noreturn)) void enter_user_mode_from_trapframe(const regs_t* tf);
process_t* process_clone(process_t* process);
void process_init_trapframe(process_t* process);
void process_init_file_descriptors(process_t* process);
uint32_t process_init_stack(process_t* process);
uint32_t process_init_heap(process_t* process);
void process_destroy(process_t* process);
uint32_t process_create_page_directory();


#endif // !INCLUDE_EXEC_PROCESS_H_
