#include "timer.h"

volatile uint32_t ticks = 0;

void timer_callback(regs_t* r) {
    ticks++;
    
    if (!current_process) {
        pic_send_eoi(IRQ_TIMER);
        schedule();
        return;
    }

    current_process->trapframe = r;
    current_process->saved_kernel_esp = (uint32_t)r;

    if (current_process->state == PROC_RUNNING) {
        current_process->ticks_left--;
    }

    if (current_process->ticks_left > 0) {
        pic_send_eoi(IRQ_TIMER);
        return; 
    }
    
    current_process->ticks_left = DEFAULT_MAX_TICKS;

    if (current_process->state == PROC_RUNNING) {
        current_process->state = PROC_READY;
        enqueue(&current_processes, &current_process);
    } 
    
    pic_send_eoi(IRQ_TIMER);
    schedule();
}

uint32_t timer_get_ticks() {
    return ticks;
}
