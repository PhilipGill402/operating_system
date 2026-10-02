#ifndef INCLUDE_EXEC_SCHEDULER_H_
#define INCLUDE_EXEC_SCHEDULER_H_

#include <stdint.h>
#include <queue.h>

#include <arch/exec/proc.h>

typedef struct process process_t;

process_t* dequeue_ready(void);
void scheduler_init(void);
void schedule_and_enter(void);
void schedule(void);
void process_wake_blocked(void);
void check_pending_signals(process_t* proc);

extern queue_t current_processes;

#endif // !INCLUDE_EXEC_SCHEDULER_H_
