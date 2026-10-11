#include "interrupts/interrupts.h"
#include "interrupts/timer.h"
#include "interrupts/plic.h"
#include "drivers/uart.h"
#include "timer.h"
#include <log.h>
#include <arch/interrupts/port.h>
#include <stdlib.h>
#include <stdio.h>

static void riscv_timer_handler(arch_trapframe_t* tf) {
    uint64_t time = riscv_read_time();
    int64_t ret = riscv_set_timer(time + timer_interval);
    if (ret < 0)
        log_error("failed to set timer\n");

    timer_callback(tf);
}

static void riscv_external_handler(arch_trapframe_t* tf) {
    uint32_t irq = arch_read_u32(PLIC_SCLAIM(0));
    if (irq == PLIC_UART0_IRQ)
        uart_handler(tf);

    if (irq != 0)
        arch_write_u32(PLIC_SCLAIM(0), irq);
}

void riscv_interrupt_dispatcher(arch_trapframe_t* tf) {
    uint64_t cause = tf->scause & SCAUSE_CODE_MASK;
    switch(cause) {
        case SCAUSE_SUPERVISOR_TIMER_IRQ: riscv_timer_handler(tf); break;
        case SCAUSE_SUPERVISOR_EXTERNAL_IRQ: riscv_external_handler(tf); break;
    }

    return;
}
