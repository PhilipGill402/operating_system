#ifndef INCLUDE_EXEC_SCHEDULER_H_
#define INCLUDE_EXEC_SCHEDULER_H_

#include <stdint.h>
#include <queue.h>

#include "exec/process.h"

__attribute__((noinline)) void context_switch(process_t *old, process_t *new);
void scheduler_init();
void schedule();

extern queue_t current_processes;

#endif // !INCLUDE_EXEC_SCHEDULER_H_
