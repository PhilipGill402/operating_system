#include "exec/scheduler.h"

queue_t current_processes;

void scheduler_init() {
    current_processes = queue_create(sizeof(process_t*));
}

process_t* dequeue_ready() {
    process_t* next;

    for (uint32_t i = 0; i < queue_size(&current_processes); i++) {
        next = *(process_t**)dequeue(&current_processes);

        if (!next) {
            return NULL;
        }

        if (next->state == PROC_READY) return next;

        enqueue(&current_processes, &next);
    } 

    return NULL;
}

void schedule() {
    process_t* next = dequeue_ready();
    
    if (!next) {
        // TODO: made some sort of idle task 
        return;
    }

    current_process = next;
    current_process->state = PROC_RUNNING;
    current_process->ticks_left = DEFAULT_MAX_TICKS;
    
    load_cr3(current_process->page_directory_phys);
    tss_set_kernel_stack(current_process->kernel_stack_top);
    enter_user_mode_from_trapframe(current_process->trapframe);
}
