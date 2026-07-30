#ifndef INCLUDE_EXEC_PROCESS_H_
#define INCLUDE_EXEC_PROCESS_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <arch/registers.h>

#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "fs/fs_types.h"
#include "memory_mapping.h"

#define MAX_FDS 128
#define MAX_ARGV 32

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

#define PAGE_SIZE 4096
#define DEFAULT_MAX_TICKS 10

#define USER_NULL_GUARD      0x00000000
#define USER_STACK_TOP       0xBFFFE000
#define USER_STACK_SIZE      (PAGE_SIZE * 16)
#define USER_STACK_BOTTOM    (USER_STACK_TOP - USER_STACK_SIZE)

#define USER_MMAP_BASE 0x40000000
#define USER_MMAP_END  0xA0000000

#define MAX_PROCESSES 64

#define SIGTERM 1
#define SIGKILL 2
#define SIGSTOP 4
#define SIGCONT 8

typedef enum {
    VMA_ANON,
    VMA_FRAMEBUFFER,
    VMA_DEVICE,
    VMA_FILE
} vm_area_type_t;

typedef struct vm_area {
    uint32_t start;
    uint32_t end;

    uint32_t prot;
    uint32_t flags;
    vm_area_type_t type;

    uint32_t* frames;
    uint32_t frame_count;
    uint8_t owns_frames;

    struct vm_area* next;
} vm_area_t;

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
    void* wait_channel;
    uint32_t entry;

    int32_t exit_status;
    uint8_t waited_on;
    uint32_t waiting_for_pid;
    uint32_t wait_result_pid;
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

    vm_area_t* vmas;
    
    regs_t* trapframe;
    uint32_t ticks_left;
    
    file_desc_t* fds[MAX_FDS];
    uint32_t open_fds;
    fs_node_t* cwd;

    uint32_t pending_signals;
    uint8_t interrupted_by_signal;
} process_t;

typedef struct {
    int argc;
    char* argv[MAX_ARGV];
} cmd_args_t;

extern process_t* current_process;
extern process_t* process_table[MAX_PROCESSES];
extern uint32_t num_processes;

__attribute__((noreturn)) void enter_user_mode_from_trapframe(const regs_t* tf);
process_t* process_clone(process_t* process);
void process_init_trapframe(process_t* process);
void process_init_file_descriptors(process_t* process);
uint32_t process_init_stack(process_t* process);
uint32_t process_add_argv_to_stack(process_t* process, cmd_args_t* args);
uint32_t process_init_heap(process_t* process);
void process_destroy(process_t* process);
uint32_t process_create_page_directory();
process_t* get_process(uint32_t pid);

#endif // !INCLUDE_EXEC_PROCESS_H_
