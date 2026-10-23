#include <arch/exec/proc.h>
#include "exec/process.h"
#include "memory/helpers.h"
#include <arch/memory/memory_map.h>
#include <stdlib.h>

process_t* current_process = NULL;
process_t* process_table[MAX_PROCESSES] = { 0 };
uint32_t num_processes = 0;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
}

process_t* get_process(uint32_t pid) {
    if (pid >= MAX_PROCESSES) return NULL;

    return process_table[pid]; 
}

void process_send_signal(process_t* proc, uint32_t sig) {
    if (!proc)
        return;

    if (!(sig & SIGTERM) && !(sig & SIGKILL) && !(sig & SIGSTOP) && !(sig & SIGCONT))
        return;
    
    proc->pending_signals |= sig;
    proc->interrupted_by_signal = 1;
    proc->state = PROC_READY;
}

process_t* process_clone(process_t* process) {
    // ERROR CHECKING
    if (!process || !process->trapframe || !process->addr_space)
        return NULL;

    
    if (!process->kernel_stack_bottom || process->kernel_stack_top != process->kernel_stack_bottom + arch_kstack_size) {
        log_error("clone: bad parent kernel stack bounds\n");
        return NULL;
    }

    uint32_t new_pid = num_processes;
    if (new_pid >= MAX_PROCESSES) {
        log_error("clone: process table full\n");
        return NULL; 
    }
    
    // Create and shallow copy
    process_t* new = kzmalloc(sizeof(process_t));
    if (!new)
        return NULL;

    memcpy(new, process, sizeof(*new));

    new->kernel_stack_bottom = 0;
    new->kernel_stack_top = 0;
    new->trapframe = NULL;
    new->addr_space = NULL;
    new->cwd = NULL;

    memset(new->fds, 0, sizeof(new->fds));
    new->open_fds = 0;
    
    // Init new kernel stack
    new->kernel_stack_bottom = (uintptr_t)kmalloc(arch_kstack_size);
    if (!new->kernel_stack_bottom) {
        kfree(new);
        return NULL;
    }
    
    new->kernel_stack_top = new->kernel_stack_bottom + arch_kstack_size;
    memset((void*)new->kernel_stack_bottom, 0, arch_kstack_size);
    
    // Init and copy over trapframe
    new->trapframe = arch_trapframe_init(new->user_stack_top, new->entry);
    arch_trapframe_copy(new->trapframe, process->trapframe);
    
    // Fork returns 0 from the child
    arch_trapframe_set_ret(new->trapframe, 0);
    
    // Clone address space
    new->addr_space = arch_address_space_clone(process->addr_space);
    if (!new->addr_space) {
        kfree((void*)new->kernel_stack_bottom);
        kfree(new);
        return NULL;
    }

    // Init child kernel context past the trapframe
    new->context = arch_context_init(new->kernel_stack_top, process_child_entry);

    // Init child metadata
    new->pid = new_pid; 
    new->ppid = process->pid;
    
    new->exit_status = 0;
    
    new->ticks_left = DEFAULT_MAX_TICKS;
    new->state = PROC_READY;

    new->pending_signals = 0;

    new->wait.type = WAIT_NONE;

    // Copy parent's open file descriptors
    for (uint32_t i = 0; i < MAX_FDS; i++) {
        file_desc_t* fd = process->fds[i];
        if (!fd)
            continue;

        fd->num_refs++;
        new->fds[i] = fd;
        new->open_fds++;
    }
    
    // Copy new name
    strcpy(new->name, process->name);

    // Deep copy current working directory
    new->cwd = fs_node_clone(process->cwd);
    
    // Add process to process table and increment num_processes
    process_table[new_pid] = new;
    num_processes++;
    
    return new;
}

uint8_t process_init_stack(process_t* process) {
    uintptr_t stack_bottom = arch_user_stack_start; 
    uintptr_t stack_top = arch_user_stack_end;

    for (uintptr_t addr = stack_bottom; addr < stack_top; addr += PAGE_SIZE) {
        uintptr_t frame = pmm_alloc_frame();
        if (!frame)
            goto fail;

        if (!arch_page_map(process->addr_space, addr, frame, ARCH_PAGE_WRITE | ARCH_PAGE_USER))
            goto fail;

        uint8_t* page = (uint8_t*)arch_phys_temp_map(frame);
        if (!page)
            goto fail;

        memset(page, 0, PAGE_SIZE);

        arch_phys_temp_unmap(page);
    }

    process->user_stack_top = stack_top;
    process->user_stack_bottom = stack_bottom;

    return 1;

fail:
    for (uint32_t addr = stack_bottom; addr < stack_top; addr += PAGE_SIZE) {
        uint32_t frame = arch_page_unmap(process->addr_space, addr);
        if (frame)
            pmm_free_frame(frame);
    }

    return 0;
}

uintptr_t process_add_argv_to_stack(process_t* process, cmd_args_t* args) {
    uintptr_t sp = process->user_stack_top;
    uintptr_t argv_ptrs[32];
    
    for (int i = args->argc - 1; i >= 0; --i) {
        size_t len = strlen(args->argv[i]) + 1;
        sp -= len;
        if (!memory_copy_to_user((void*)sp, args->argv[i], len, process->addr_space))
            return 0;

        argv_ptrs[i] = sp;
    }

    sp &= ~0xF;

    sp -= sizeof(uintptr_t);
    uint32_t zero = 0;
    if (!memory_copy_to_user((void*)sp, &zero, sizeof(uintptr_t), process->addr_space))
        return 0;

    for (int i = args->argc - 1; i >= 0; i--) {
        sp -= sizeof(uintptr_t);
        if (!memory_copy_to_user((void*)sp, &argv_ptrs[i], sizeof(uintptr_t), process->addr_space))
            return 0;
    }

    sp -= sizeof(uintptr_t);
    
    if (!memory_copy_to_user((void*)sp, &args->argc, sizeof(uintptr_t), process->addr_space))
        return 0;

    return sp;
}

uint32_t process_init_heap(process_t* process) {
    uint8_t frame_allocated = 0;
    uint8_t page_allocated = 0;
    uint8_t page_mapped = 0;
    
    uintptr_t frame = pmm_alloc_frame();
    if (!frame)
        goto fail;

    frame_allocated = 1;
    
    if (!arch_page_map(process->addr_space, arch_user_heap_start, frame, ARCH_PAGE_WRITE | ARCH_PAGE_USER))
        goto fail;

    page_allocated = 1;
    
    uint8_t* page = (uint8_t*)arch_phys_temp_map(frame);
    if (!page)
        goto fail;

    page_mapped = 1;

    memset(page, 0, PAGE_SIZE);

    process->heap_start = arch_user_heap_start;
    process->heap_break = process->heap_start;
    process->heap_mapped_end = process->heap_start + PAGE_SIZE;
    process->heap_max_end = arch_user_heap_end;
                                                              
    if (process->heap_max_end >= USER_MMAP_BASE)
        process->heap_max_end = USER_MMAP_BASE - 1;
    
    arch_phys_temp_unmap(page);
    
    return 1;

fail:
    if (page_mapped)
        arch_phys_temp_unmap(page);
    
    if (page_allocated)
        arch_page_unmap(process->addr_space, arch_user_heap_start);
    
    if (frame_allocated)
        pmm_free_frame(frame);
    
    return 0;
}

void process_init_file_descriptors(process_t* process) {
    memset(process->fds, 0, sizeof(file_desc_t*) * MAX_FDS);
    fs_node_t* console = resolve_path("/dev/console", fs_root);

    if (!console) {
        log_error("couldn't find console\n");
        return;
    }
    
    file_desc_t* fd = fs_create_file_desc(console, 0);

    for (uint8_t i = 0; i < 3; i++)
        process->fds[i] = fd;
    
    fd->num_refs = 3;

    process->open_fds = 3;

    kfree(console);
}


void process_destroy(process_t* process) {
    // cleans up all the mapped virtual memory
    for (uint8_t i = 0; i < process->num_ranges; i++) {
        uint32_t start = process->mem_ranges[i].start;
        uint32_t end = process->mem_ranges[i].end;

        for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
            uint32_t frame = arch_page_unmap(process->addr_space, addr);
            if (frame) {
                pmm_free_frame(frame);
            }
        }
    }

    // cleans up process virtual memory
    vm_area_t* vma = process->vmas;
    while (vma) {
        vm_area_t* next = vma->next;

        for (uint32_t i = 0; i < vma->frame_count; i++) {
            uint32_t virt = vma->start + i * PAGE_SIZE;

            uint32_t frame = arch_page_unmap(process->addr_space, virt);

            if (vma->owns_frames && frame) {
                if (vma->frames[i] && vma->frames[i] == frame)
                    pmm_free_frame(vma->frames[i]);
            }
        }

        kfree(vma->frames);
        
        kfree(vma);
        vma = next;
    }

    // cleans up the user stack memory
    for (uint32_t addr = process->user_stack_bottom; addr < process->user_stack_top; addr += PAGE_SIZE) {
        uint32_t frame = arch_page_unmap(process->addr_space, addr);
        if (frame)
            pmm_free_frame(frame);
    }

    // cleans up heap memory
    for (uint32_t addr = process->heap_start; addr < process->heap_mapped_end; addr += PAGE_SIZE) {
        uint32_t frame = arch_page_unmap(process->addr_space, addr);

        if (frame)
            pmm_free_frame(frame);
    }
    
    // free all file descriptor nodes
    for (uint32_t i = 0; i < MAX_FDS; i++) {
        if (process->fds[i])
            fs_free_file_desc(process->fds[i]);            

        memset(&process->fds[i], 0, sizeof(file_desc_t*));
    }

    if (current_process == process) {
        arch_address_space_activate(arch_kernel_address_space());
        current_process = NULL;
    }

    arch_address_space_destroy(process->addr_space);
    
    // frees heap allocated variables
    kfree(process->cwd);
    
    if (process->trapframe)
        arch_trapframe_destroy(process->trapframe);

    if (process->context)
        arch_context_destroy(process->context);
    
    if (process->kernel_stack_bottom)
        kfree((void*)process->kernel_stack_bottom);
    
    process_table[process->pid] = NULL;

    kfree(process);
}

void process_child_entry(void) {
    process_t* child = current_process;
    if (!child || !child->trapframe) {
        log_error("child entry: invalid process\n");
        for (;;)
            arch_halt();
    } 

    arch_return_to_user(child->trapframe, child->kernel_stack_top);

    log_error("arch_return_to_user returned\n");
    for (;;)
        arch_halt();

}

_Noreturn void process_first_entry(void) {
    if (!current_process || !current_process->trapframe)
        panic("can't enter null process or null trapframe\n");

    arch_return_to_user(current_process->trapframe, current_process->kernel_stack_top);
}

