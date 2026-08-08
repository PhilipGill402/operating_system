#include "exec/scheduler.h"

#include "fs/fs.h"
#include <arch/exec/user_mode.h>
#include <arch/cpu/tss.h>

queue_t current_processes;

static process_t* idle_process;

static _Noreturn void idle_entry(void) {
    current_process = idle_process;
    idle_process->state = PROC_IDLE; 
    for (;;) {
        arch_enable_interrupts();
        arch_halt();
        
        schedule();
    }
}

void scheduler_init(void) {
    current_processes = queue_create(sizeof(process_t*));
    
    idle_process = kmalloc(sizeof(process_t));
    memset(idle_process, 0, sizeof(process_t));
    idle_process->pid = UINT32_MAX;
    idle_process->state = PROC_READY;

    idle_process->kernel_stack_bottom = (uintptr_t)kmalloc(KERNEL_STACK_SIZE);
    idle_process->kernel_stack_top = idle_process->kernel_stack_bottom + KERNEL_STACK_SIZE;
    
    idle_process->context = arch_context_init(idle_process->kernel_stack_top, idle_entry);
}

void check_pending_signals(process_t* proc) {
    if (proc->pending_signals & SIGKILL) {
        log_debug("killing process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= ~SIGKILL;
        proc->exit_status = 128 + SIGKILL;
        proc->state = PROC_TERMINATED;
        return;
    } 
    
    if (proc->pending_signals & SIGTERM) {
        log_debug("terminating process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= ~SIGTERM;
        proc->exit_status = 128 + SIGTERM; 
        proc->state = PROC_TERMINATED;
        return;
    } 
    
    if (proc->pending_signals & SIGSTOP) {
        log_debug("stopping process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= ~SIGSTOP;
        proc->state = PROC_BLOCKED;
        return;
    } 

    if (proc->pending_signals & SIGCONT) {
        log_debug("continuing process %s (%d)\n", proc->name, proc->pid);
        proc->pending_signals &= ~SIGCONT; 
        if (proc->state == PROC_BLOCKED) 
            proc->state = PROC_READY;
    }
}

process_t* dequeue_ready(void) {
    process_t* next;
    
    uint32_t size = queue_size(&current_processes);
    for (uint32_t i = 0; i < size; i++) {
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

void schedule_and_enter(void) {
    process_t* next = dequeue_ready();
         
    if (!next) {
        // switch to idle process
        arch_context_switch(current_process->context, idle_process->context);
        return;
    }
    
    current_process = next;
    current_process->state = PROC_RUNNING;
    current_process->ticks_left = DEFAULT_MAX_TICKS;
    
    arch_address_space_activate(current_process->addr_space);
    arch_set_kernel_stack(current_process->kernel_stack_top);
    arch_return_to_user(current_process->trapframe);
}

void schedule(void) {
    process_t* outgoing = current_process;

    if (!outgoing) {
        log_error("no current process\n");
        return;
    }

    if (outgoing->state == PROC_RUNNING)
        outgoing->state = PROC_READY;

    if (outgoing->state == PROC_READY)
        enqueue(&current_processes, &outgoing);
    
    process_wake_blocked();
    process_t* incoming = dequeue_ready();

    if (!incoming) {
        if (outgoing->state == PROC_READY) {
            outgoing->state = PROC_RUNNING;
            return;
        }
        // TODO: implement idle task
        current_process = idle_process; 
        arch_context_switch(outgoing->context, idle_process->context);
        return;
    }
    
    current_process = incoming;
    incoming->state = PROC_RUNNING;
    incoming->ticks_left = DEFAULT_MAX_TICKS;

    arch_address_space_activate(incoming->addr_space);
    arch_set_kernel_stack(incoming->kernel_stack_top);
    arch_context_switch(outgoing->context, incoming->context);
}

static uint8_t process_is_blocked_ready(process_t* proc) {
    if (!proc || proc->state != PROC_BLOCKED)
        return 0;

    switch (proc->wait.type) {
        case WAIT_FD: {
            uint32_t fd = proc->wait.fd.fd;

            if (fd >= MAX_FDS)
                return 1;

            file_desc_t* desc = proc->fds[fd];

            if (!desc || !desc->node)
                return 1;

            uint32_t events = fs_poll(desc->node, desc->offset);

            return (events & proc->wait.fd.events) != 0;
        }

        case WAIT_CHILD: {
            process_t* child = get_process(proc->wait.child.pid);

            return !child || child->state == PROC_TERMINATED;
        }

        case WAIT_POLL: {
            for (uint32_t i = 0; i < proc->wait.poll.nfds; ++i) {
                uint32_t fd = proc->wait.poll.fds[i].fd;
                uint32_t event_mask = proc->wait.poll.fds[i].events;
                
                if (fd >= MAX_FDS)
                    continue;

                file_desc_t* desc = proc->fds[fd];

                if (!desc || !desc->node)
                    continue;

                if (fs_poll(desc->node, desc->offset) & event_mask)
                    return 1;
            } 

            return 0;
        }

        case WAIT_CHANNEL:
        case WAIT_NONE:
        default:
            return 0;
    }
}

void process_wake_blocked(void) {
    for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
        process_t* proc = process_table[i];
        if (!proc)
            continue;

        if (proc->state != PROC_BLOCKED)
            continue;

        if (!process_is_blocked_ready(proc))
            continue;
        
        proc->state = PROC_READY;
        enqueue(&current_processes, &proc);
    }
}

