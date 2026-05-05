#include "exec/scheduler.h"

queue_t current_processes;

extern uint8_t debug_sched;
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

process_t* get_process(uint32_t pid) {
    if (pid >= MAX_PROCESSES) return NULL;

    return process_table[pid]; 
}

void process_wake_parent(uint32_t child_pid) {
    process_t* child = get_process(child_pid);
    if (!child) return;

    process_t* parent = get_process(child->ppid);
    if (!parent) return;
    
    if (parent->state == PROC_BLOCKED && parent->waiting_for_pid == child_pid) {
        parent->state = PROC_READY;
        parent->waiting_for_pid = 0;

        enqueue(&current_processes, &parent);
    }
}
