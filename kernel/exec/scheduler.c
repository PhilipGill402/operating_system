#include "exec/scheduler.h"

queue_t current_processes;

__attribute__((noinline))
void context_switch(process_t *old, process_t *new) {
    __asm__ volatile (
        // Save callee-saved registers on current stack
        "pushl %%ebp\n\t"
        "pushl %%ebx\n\t"
        "pushl %%esi\n\t"
        "pushl %%edi\n\t"

        // Save current esp into old->esp
        "movl %%esp, (%0)\n\t"

        // Load new->esp into esp
        "movl (%1), %%esp\n\t"

        // Restore callee-saved registers from new stack
        "popl %%edi\n\t"
        "popl %%esi\n\t"
        "popl %%ebx\n\t"
        "popl %%ebp\n\t"

        // Return to new task's saved EIP
        "ret\n\t"
        :
        : "r" (old), "r" (new)
        : "memory"
    );
}

void scheduler_init() {
    current_processes = queue_create(sizeof(process_t*));
}

process_t* dequeue_ready() {
    process_t* next;

    for (uint32_t i = 0; i < queue_size(&current_processes); i++) {
        next = dequeue(&current_processes);

        if (next->state == PROC_READY) return next;

        enqueue(&current_processes, next);
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
    enter_user_mode_from_trapframe(current_process->trapframe);
}
