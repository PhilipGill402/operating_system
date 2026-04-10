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
