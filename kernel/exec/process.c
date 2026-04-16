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

uint32_t clone_page_directory(uint32_t* parent_page_directory) {
    uint32_t* child_dir = process_create_page_directory();

    for (uint32_t pd_idx = 0; pd_idx < 768; pd_idx++) {
        if (!(parent_page_directory[pd_idx] & PAGE_PRESENT)) continue;
        if (!(parent_page_directory[pd_idx] & PAGE_USER)) continue;

        uint32_t parent_pt_phys = parent_page_directory[pd_idx] & 0xFFFFF000;
        uint32_t* parent_pt = temp_map_phys0(parent_pt_phys);

        for (uint32_t pt_idx = 0; pt_idx < 1024; pt_idx++) {
            if (!(parent_pt[pt_idx] & PAGE_PRESENT)) continue;

            uint32_t parent_frame = parent_pt[pt_idx] & 0xFFFFF000;
            uint32_t flags = parent_pt[pt_idx] & 0xFFF;

            uint32_t child_frame = pmm_alloc_frame();
            if (!child_frame) {
                return 0;
            }

            uint32_t* src = temp_map_phys0(parent_frame);
            uint32_t* dst = temp_map_phys1(child_frame);
            memcpy(dst, src, PAGE_SIZE);

            uint32_t virt = (pd_idx << 22) | (pt_idx << 12);
            map_user_page(child_dir, virt, child_frame, flags);
        }
    }

    return child_dir;
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

    new->kernel_stack_top = new->kernel_stack_bottom + KERNEL_STACK_SIZE;
    memcpy((void*)new->kernel_stack_bottom, (void*)process->kernel_stack_bottom, KERNEL_STACK_SIZE);
    
    uint32_t delta = new->kernel_stack_bottom - process->kernel_stack_bottom;
    new->saved_kernel_esp = process->saved_kernel_esp + delta;
    new->saved_kernel_ebp = process->saved_kernel_ebp + delta;
    new->trapframe = (regs_t*)((uint32_t)process->trapframe + delta);

    new->pid = num_processes++;
    new->ticks_left = DEFAULT_MAX_TICKS;
    new->state = PROC_READY;
    new->page_directory_phys = clone_page_directory(process->page_directory_phys);

    if (!new->page_directory_phys) {
        return NULL;
    }

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

void process_init_file_descriptors(process_t* process) {
    memset(&process->fds, 0, sizeof(file_desc_t) * MAX_FDS);
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
    for (uint32_t addr = current_process->user_stack_bottom; addr < current_process->user_stack_top; addr += PAGE_SIZE) {
        uint32_t frame = unmap_page(addr);
        if (frame) {
            pmm_free_frame(frame);
        }
    }
    
    // frees heap allocated variables
    kfree(process->cwd);
    kfree(process);
    kfree(process->kernel_stack_bottom);
    process = NULL;
}
