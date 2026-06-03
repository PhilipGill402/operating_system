#ifndef INCLUDE_EXEC_SCHEDULER_H_
#define INCLUDE_EXEC_SCHEDULER_H_

#include <stdint.h>
#include <queue.h>

#include "exec/process.h"
#include "exec/tss.h"

process_t* dequeue_ready();
void scheduler_init();
void schedule_and_enter();
void schedule_from_interrupt(regs_t* r);
void process_wake_parent(uint32_t child_pid);

extern queue_t current_processes;

#endif // !INCLUDE_EXEC_SCHEDULER_H_
