#include "timer.h"

volatile uint32_t ticks = 0;

void timer_callback(regs_t* r) {
    ticks++;
    

    if (framebuffer_initialized && ticks % 50 == 0) {
        cursor_on = !cursor_on;
        
        /* --- CHANGE THIS TOO SLOW --- */
        framebuffer_clear_cursor();

        if (cursor_on) {
            framebuffer_draw_cursor();
        }
        
        framebuffer_flush();
    }

    if (!current_process) {
        pic_send_eoi(IRQ_TIMER);
        return;
    }


    memcpy(current_process->trapframe, r, sizeof(regs_t)); 
    //current_process->saved_kernel_esp = (uint32_t)r;

    if (current_process->ticks_left > 0 && current_process->state == PROC_RUNNING) {
        current_process->ticks_left--;
    }
    
    if (current_process->ticks_left == 0) {
        current_process->ticks_left = DEFAULT_MAX_TICKS;

        if ((r->cs & 0x3) == 0x3) {
            pic_send_eoi(IRQ_TIMER);
            schedule_from_interrupt(r);
            return;
        }
    } 

    pic_send_eoi(IRQ_TIMER);
}

uint32_t timer_get_ticks() {
    return ticks;
}
