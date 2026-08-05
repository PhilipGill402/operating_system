#ifndef INCLUDE_EXEC_SCHEDULER_H_
#define INCLUDE_EXEC_SCHEDULER_H_

#include <stdint.h>
#include <queue.h>

#include <arch/exec/proc.h>
#include <arch/cpu/tss.h>

#include "exec/process.h"

process_t* dequeue_ready();
void scheduler_init();
void schedule_and_enter();
void schedule_from_interrupt(arch_trapframe_t* tf);
void process_wake_parent(uint32_t child_pid);
void process_block_current_process(void* channel);
void process_wake_blocked(void* channel);

extern queue_t current_processes;

#endif // !INCLUDE_EXEC_SCHEDULER_H_
