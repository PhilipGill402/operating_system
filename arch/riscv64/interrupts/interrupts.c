#include "interrupts/interrupts.h"
#include "interrupts/timer.h"
#include "timer.h"
#include <log.h>

void riscv_timer_handler(arch_trapframe_t* tf) {
    uint64_t time = riscv_read_time();
    int64_t ret = riscv_set_timer(time + timer_interval);
    if (ret < 0)
        log_error("failed to set timer\n");

    timer_callback(tf);
}

void riscv_interrupt_dispatcher(arch_trapframe_t* tf) {
    uint64_t cause = tf->scause & SCAUSE_CODE_MASK;
    switch(cause) {
        case SCAUSE_SUPERVISOR_TIMER_IRQ: riscv_timer_handler(tf);
    }

    return;
}
