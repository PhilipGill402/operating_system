#include "exec/scheduler.h"

queue_t current_processes;

void scheduler_init() {
    current_processes = queue_create(sizeof(process_t*));
}


void complete_pending_wait(process_t* process) {
    if (!process->wait_has_results) return;
    
    if (process->waiting_status_ptr) {
        *(process->waiting_status_ptr) = process->wait_result_status;
    }
    
    process->trapframe->eax = process->wait_result_pid;

    process->wait_has_results = 0;
    process->wait_result_pid = 0; 
    process->waiting_status_ptr = NULL;
    process->waiting_for_pid = 0;
}

void check_pending_signals(process_t* proc) {
    if (proc->pending_signals & SIGTERM) {
        log_debug("terminating process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= SIGTERM;
        proc->exit_status = 128 + SIGTERM; 
        proc->state = PROC_TERMINATED;
        process_wake_parent(proc->pid);
    } 
    
    if (proc->pending_signals & SIGKILL) {
        log_debug("killing process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= SIGKILL;
        proc->exit_status = 128 + SIGKILL;
        proc->state = PROC_TERMINATED;
        process_wake_parent(proc->pid);
    } 
    
    if (proc->pending_signals & SIGSTOP) {
        log_debug("stopping process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= SIGSTOP;
        proc->state = PROC_BLOCKED;
    } 

    if (proc->pending_signals & SIGCONT) {
        log_debug("continuing process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= SIGCONT; 
        proc->state = PROC_READY;
    }
}

process_t* dequeue_ready() {
    process_t* next;

    for (uint32_t i = 0; i < queue_size(&current_processes); i++) {
        next = *(process_t**)dequeue(&current_processes);

        if (!next) {
            return NULL;
        }

        check_pending_signals(next);
        
        if (next->state == PROC_READY)
            return next;

        enqueue(&current_processes, &next);
    } 

    return NULL;
}

void scheduler_idle_loop() {
    current_process = NULL;

    for (;;) {
        asm volatile("sti; hlt");
        process_t* next = dequeue_ready();

        if (!next) {
            continue;
        }

        current_process = next;
        current_process->state = PROC_RUNNING;
        current_process->ticks_left = DEFAULT_MAX_TICKS;

        load_cr3(current_process->page_directory_phys);
        tss_set_kernel_stack(current_process->kernel_stack_top);
        complete_pending_wait(current_process);

        enter_user_mode_from_trapframe(current_process->trapframe);
    }
}

void schedule_and_enter() {
    process_t* next = dequeue_ready();
         
    if (!next) {
        // TODO: made some sort of idle task 
        scheduler_idle_loop();
        return;
    }
    
    check_pending_signals(next);

    current_process = next;
    current_process->state = PROC_RUNNING;
    current_process->ticks_left = DEFAULT_MAX_TICKS;
    
    load_cr3(current_process->page_directory_phys);
    tss_set_kernel_stack(current_process->kernel_stack_top);
    complete_pending_wait(next);
    enter_user_mode_from_trapframe(current_process->trapframe);
}

void schedule_from_interrupt(regs_t* r) {
    if (!current_process || !r)
        return;

    memcpy(current_process->trapframe, r, sizeof(regs_t));
    
    if (current_process->state == PROC_RUNNING) {
        check_pending_signals(current_process);

        if (current_process->state == PROC_RUNNING) {
            current_process->state = PROC_READY;
        }

        if (current_process->state == PROC_READY) {
            enqueue(&current_processes, &current_process);
        }
    }

    process_t* next = dequeue_ready();

    if (!next) {
        if (current_process->state == PROC_READY || current_process->state == PROC_RUNNING) {
            current_process->state = PROC_RUNNING;
            current_process->ticks_left = DEFAULT_MAX_TICKS;
            memcpy(r, current_process->trapframe, sizeof(regs_t));
            return;
        } 
        
        current_process = NULL;
        scheduler_idle_loop();
    }

    //log_debug("switching to process %s (%d)\n", next->name, next->pid);

    check_pending_signals(next);

    current_process = next;
    current_process->state = PROC_RUNNING;
    current_process->ticks_left = DEFAULT_MAX_TICKS;

    load_cr3(current_process->page_directory_phys);
    tss_set_kernel_stack(current_process->kernel_stack_top);
    complete_pending_wait(current_process);
    
    uint32_t saved_ds = r->ds;
    memcpy(r, current_process->trapframe, sizeof(regs_t));
    r->ds = saved_ds;
}



void process_wake_parent(uint32_t child_pid) {
    process_t* child = get_process(child_pid);
    if (!child) return;

    process_t* parent = get_process(child->ppid);
    if (!parent) return;
    
    if (parent->state == PROC_BLOCKED && parent->waiting_for_pid == child_pid) {
        parent->state = PROC_READY;
        parent->waiting_for_pid = 0;
        parent->wait_result_pid = child_pid;
        parent->wait_result_status = child->exit_status;
        parent->wait_has_results = 1;

        enqueue(&current_processes, &parent);
        process_destroy(child);
    }
}


