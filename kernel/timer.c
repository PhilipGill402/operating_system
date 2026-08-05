#include "timer.h"

volatile uint32_t ticks = 0;

void timer_callback(arch_trapframe_t* tf) {
    ticks++;
    
    if (!current_process) {
        pic_send_eoi(IRQ_TIMER);
        return;
    }

    arch_trapframe_copy(current_process->trapframe, tf); 
    
    if (current_process->ticks_left > 0 && current_process->state == PROC_RUNNING) {
        current_process->ticks_left--;
    }
    
    if (current_process->ticks_left == 0) {
        current_process->ticks_left = DEFAULT_MAX_TICKS;

        if (arch_trapframe_from_user(tf)) {
            pic_send_eoi(IRQ_TIMER);
            schedule_from_interrupt(tf);
            return;
        }
    } 

    pic_send_eoi(IRQ_TIMER);
}

uint32_t timer_get_ticks() {
    return ticks;
}
