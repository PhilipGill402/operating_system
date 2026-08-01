#include "exec/process.h"

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

void process_init_trapframe(process_t* process) {
    regs_t* tf = (regs_t*)(process->kernel_stack_top - sizeof(regs_t));
    memset(tf, 0, sizeof(regs_t));

    tf->eip = process->entry;
    tf->cs = USER_CS_RING3;
    tf->eflags = 0x202;
    tf->useresp = process->user_stack_top;
    tf->ss = USER_DS_RING3;
    tf->ds = USER_DS_RING3;

    process->trapframe = kmalloc(sizeof(regs_t)); 
    memcpy(process->trapframe, tf, sizeof(regs_t));

    process->saved_kernel_esp = (uint32_t)tf;
    process->saved_kernel_ebp = 0;

}

process_t* process_clone(process_t* process) {
    process_t* new = kzmalloc(sizeof(process_t));
    
    if (!new)
        return NULL;

    memcpy(new, process, sizeof(process_t));
    
    new->kernel_stack_bottom = (uint32_t)kmalloc(KERNEL_STACK_SIZE);
    
    if (!new->kernel_stack_bottom) {
        kfree(new);
        return NULL;
    }
    
    if (!process->kernel_stack_bottom || process->kernel_stack_top != process->kernel_stack_bottom + KERNEL_STACK_SIZE) {
        log_error("clone: bad parent kernel stack bounds\n");
        kfree((void*)new->kernel_stack_bottom);
        kfree(new);
        return NULL;
    }
    
    new->kernel_stack_top = new->kernel_stack_bottom + KERNEL_STACK_SIZE;
    memcpy((void*)new->kernel_stack_bottom, (void*)process->kernel_stack_bottom, KERNEL_STACK_SIZE);
    
    uint32_t delta = new->kernel_stack_bottom - process->kernel_stack_bottom;
    new->saved_kernel_esp = process->saved_kernel_esp + delta;
    new->saved_kernel_ebp = process->saved_kernel_ebp + delta;
    
    new->trapframe = kzmalloc(sizeof(regs_t));
    if (!new->trapframe) {
        kfree((void*)new->kernel_stack_bottom);
        kfree(new);
        return NULL;
    }

    memcpy(new->trapframe, process->trapframe, sizeof(regs_t));
    new->trapframe->eax = 0;
    
    new->pid = num_processes++;
    new->ppid = process->pid;
    
    new->exit_status = 0;
    new->waited_on = 0;
    new->waiting_for_pid = 0;
    new->wait_result_pid = 0;
    new->wait_result_status = 0;
    new->waiting_status_ptr = NULL;
    new->wait_has_results = 0;

    new->ticks_left = DEFAULT_MAX_TICKS;
    new->state = PROC_READY;

    new->pending_signals = 0;

    new->wait_channel = NULL;
    
    new->addr_space = arch_address_space_clone(process->addr_space);
    if (!new->addr_space) {
        kfree(new->trapframe);
        kfree((void*)new->kernel_stack_bottom);
        kfree(new);
        return NULL;
    }
    
    
    memset(new->fds, 0, sizeof(new->fds));
    new->open_fds = 0;

    for (uint32_t i = 0; i < MAX_FDS; i++) {
        file_desc_t* fd = process->fds[i];
        if (!fd)
            continue;

        fd->num_refs++;
        new->fds[i] = fd;
        new->open_fds++;
    }

    strcpy(new->name, process->name);
    new->cwd = fs_node_clone(process->cwd);
    
    process_table[new->pid] = new;
    
    return new;
}

uint32_t process_init_stack(process_t* process) {
    uint32_t stack_bottom = USER_STACK_BOTTOM; 
    uint32_t stack_top = USER_STACK_TOP;

    for (uint32_t addr = stack_bottom; addr < stack_top; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame)
            goto fail;

        if (!arch_page_map(process->addr_space, addr, frame, ARCH_PAGE_WRITE | ARCH_PAGE_USER))
            goto fail;

        uint8_t* page = (uint8_t*)arch_phys_temp_map(1, frame);
        if (!page)
            goto fail;

        memset(page, 0, PAGE_SIZE);

        arch_phys_temp_unmap(1);
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

uint32_t process_add_argv_to_stack(process_t* process, cmd_args_t* args) {
    uint32_t sp = process->user_stack_top;
    uint32_t argv_ptrs[32];
    
    for (int i = args->argc - 1; i >= 0; --i) {
        size_t len = strlen(args->argv[i]) + 1;
        sp -= len;
        memcpy((void*)sp, args->argv[i], len);
        argv_ptrs[i] = sp;
    }

    sp &= ~0xF;

    sp -= sizeof(uint32_t);
    *(uint32_t*)sp = 0;

    for (int i = args->argc - 1; i >= 0; i--) {
        sp -= sizeof(uint32_t);
        *(uint32_t*)sp = argv_ptrs[i];
    }

    sp -= sizeof(uint32_t);
    *(uint32_t*)sp = args->argc;

    return sp;
}

uint32_t process_init_heap(process_t* process) {
    uint32_t heap_start = align_up(process->image_end, PAGE_SIZE);

    uint8_t frame_allocated = 0;
    uint8_t page_allocated = 0;
    uint8_t page_mapped = 0;
    
    uint32_t frame = pmm_alloc_frame();
    if (!frame)
        goto fail;

    frame_allocated = 1;

    if (!arch_page_map(process->addr_space, heap_start, frame, ARCH_PAGE_WRITE | ARCH_PAGE_USER))
        goto fail;

    page_allocated = 1;

    uint8_t* page = arch_phys_temp_map(0, frame);
    if (!page)
        goto fail;

    page_mapped = 1;

    memset(page, 0, PAGE_SIZE);

    process->heap_start = heap_start;
    process->heap_break = process->heap_start;
    process->heap_mapped_end = process->heap_start + PAGE_SIZE;
    process->heap_max_end = heap_start + (16384 * PAGE_SIZE); // allow heap growth up to 64 MB
                                                              
    if (process->heap_max_end >= USER_MMAP_BASE)
        process->heap_max_end = USER_MMAP_BASE - 1;
    
    arch_phys_temp_unmap(0);
    
    return 1;

fail:
    if (page_mapped)
        arch_phys_temp_unmap(0);
    
    if (page_allocated)
        arch_page_unmap(process->addr_space, heap_start);
    
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
    kfree(process->kernel_stack_bottom);
    
    process_table[process->pid] = NULL;

    kfree(process);
}

