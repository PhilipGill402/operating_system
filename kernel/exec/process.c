#include "exec/process.h"

process_t* current_process = NULL;
process_t* process_table[MAX_PROCESSES] = { 0 };
uint32_t num_processes = 0;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
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

    process->trapframe = tf;
    process->saved_kernel_esp = (uint32_t)tf;
    process->saved_kernel_ebp = 0;

}

uint32_t clone_page_directory(uint32_t parent_page_directory_phys) {
    uint32_t child_pd_phys = process_create_page_directory();
    uint32_t* parent_pd = temp_map_phys0(parent_page_directory_phys);
    uint32_t* child_pd  = temp_map_phys1(child_pd_phys);

    for (uint32_t pd_idx = 0; pd_idx < 768; pd_idx++) {
        if (!(parent_pd[pd_idx] & PAGE_PRESENT)) continue;
        if (!(parent_pd[pd_idx] & PAGE_USER)) continue;

        uint32_t parent_pt_phys = parent_pd[pd_idx] & 0xFFFFF000;
        uint32_t* parent_pt = temp_map_phys2(parent_pt_phys);
        
        for (uint32_t pt_idx = 0; pt_idx < 1024; pt_idx++) {
            uint32_t pte = parent_pt[pt_idx];

            if (!(pte & PAGE_PRESENT)) continue;

            uint32_t parent_frame = pte & 0xFFFFF000;
            uint32_t flags = pte & 0xFFF;

            uint32_t child_frame = pmm_alloc_frame();
            if (!child_frame) {
                return 0;
            }

            uint32_t* src = temp_map_phys3(parent_frame);
            uint32_t* dst = temp_map_phys4(child_frame);
            memcpy(dst, src, PAGE_SIZE);

            uint32_t virt = (pd_idx << 22) | (pt_idx << 12);
            
            map_user_page(child_pd, virt, child_frame, flags);
        }
    }

    

    return child_pd_phys;
}

process_t* process_clone(process_t* process) {
    process_t* new = kzmalloc(sizeof(process_t));
    
    
    if (!new) {
        return NULL;
    }

    memcpy(new, process, sizeof(process_t));
    
    new->kernel_stack_bottom = (uint32_t)kmalloc(KERNEL_STACK_SIZE);
    
    if (!new->kernel_stack_bottom) {
        return NULL;
    }
    
    if (!process->kernel_stack_bottom || process->kernel_stack_top != process->kernel_stack_bottom + KERNEL_STACK_SIZE) {
        log_error("clone: bad parent kernel stack bounds\n");
        kfree(new);
        return NULL;
    }
    
    new->kernel_stack_top = new->kernel_stack_bottom + KERNEL_STACK_SIZE;
    memcpy((void*)new->kernel_stack_bottom, (void*)process->kernel_stack_bottom, KERNEL_STACK_SIZE);
    
    uint32_t delta = new->kernel_stack_bottom - process->kernel_stack_bottom;
    new->saved_kernel_esp = process->saved_kernel_esp + delta;
    new->saved_kernel_ebp = process->saved_kernel_ebp + delta;
    new->trapframe = (regs_t*)((uint32_t)process->trapframe + delta);
    
    new->pid = num_processes++;
    new->ppid = process->pid;
    
    new->exit_status = 0;
    new->waited_on = 0;
    new->waiting_for_pid = 0;
    new->wait_result_status = 0;
    new->waiting_status_ptr = NULL;
    new->wait_has_results = 0;

    new->ticks_left = DEFAULT_MAX_TICKS;
    new->state = PROC_READY;

    new->page_directory_phys = clone_page_directory(process->page_directory_phys);
    
    if (!new->page_directory_phys) {
        return NULL;
    }

    strcpy(new->name, process->name);
    new->cwd = fs_node_clone(process->cwd);
    
    process_table[new->pid] = new;
    
    return new;
}

uint32_t process_create_page_directory() {
    uint32_t pd_phys = pmm_alloc_frame();
    
    if (!pd_phys) {
        return 0;
    }

    uint32_t* pd = temp_map_phys0(pd_phys);
    memset(pd, 0, PAGE_SIZE);

    for (uint32_t i = 768; i < 1024; i++) {
        pd[i] = kernel_page_directory[i];
    }

    return pd_phys;
}

uint32_t process_init_stack(process_t* process) {
    uint32_t stack_bottom = USER_STACK_BOTTOM; 
    uint32_t stack_top = USER_STACK_TOP;

    uint32_t* pd = temp_map_phys0(process->page_directory_phys);

    for (uint32_t addr = stack_bottom; addr < stack_top; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return 0;
        }

        map_user_page(pd, addr, frame, PAGE_WRITE);

        uint8_t* page = (uint8_t*)temp_map_phys1(frame);
        memset(page, 0, PAGE_SIZE);
    }

    process->user_stack_top = stack_top;
    process->user_stack_bottom = stack_bottom;

    return 1;
}

uint32_t process_init_heap(process_t* process) {
    uint32_t heap_start = align_up(process->image_end, PAGE_SIZE);
    
    uint32_t* pd = temp_map_phys0(process->page_directory_phys);
    uint32_t frame = pmm_alloc_frame();
    if (!frame) {
        return 0;
    }

    map_user_page(pd, heap_start, frame, PAGE_WRITE);
    
    uint8_t* page = temp_map_phys1(frame);
    memset(page, 0, PAGE_SIZE);

    process->heap_start = heap_start;
    process->heap_break = process->heap_start;
    process->heap_mapped_end = process->heap_start + PAGE_SIZE;
    process->heap_max_end = heap_start + (16384 * PAGE_SIZE); // allow heap growth up to 64 MB
                                                             
    return 1;
}

void process_init_file_descriptors(process_t* process) {
    memset(process->fds, 0, sizeof(file_desc_t) * MAX_FDS);
    fs_node_t* console = resolve_path("/dev/console");

    for (uint8_t i = 0; i < 3; i++) {
        process->fds[i].in_use = 1; 
        process->fds[i].node = console;
        process->fds[i].offset = 0;
        process->fds[i].flags = 0;
    }

    process->open_fds = 3;
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

    // cleans up the user stack memory
    for (uint32_t addr = process->user_stack_bottom; addr < process->user_stack_top; addr += PAGE_SIZE) {
        uint32_t frame = unmap_page(addr);
        if (frame) {
            pmm_free_frame(frame);
        }
    }
    
    // frees heap allocated variables
    kfree(process->cwd);
    kfree(process->kernel_stack_bottom);
    kfree(process);
    process = NULL;
}





