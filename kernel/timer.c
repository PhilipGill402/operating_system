#include "timer.h"

volatile uint32_t ticks = 0;

void timer_callback(regs_t* r) {
    ticks++;

    if (current_process) {
        current_process->trapframe = r;
        current_process->ticks_left--;
        if (current_process->ticks_left == 0) {
            printf("swithc!\n");
        }
    }
    
}

uint32_t timer_get_ticks() {
    return ticks;
}
