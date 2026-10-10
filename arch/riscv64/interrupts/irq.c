#include "interrupts/irq.h"
#include "interrupts/timer.h"
#include "interrupts/plic.h"
#include "interrupts/exceptions.h"
#include "interrupts/interrupts.h"
#include "trapframe.h"
#include <arch/asm/helpers.h>
#include <stdint.h>
#include <log.h>
#include <stdlib.h>

extern void riscv_trap_entry(void);

void riscv_trap_handler(arch_trapframe_t* tf) {
    if (tf->scause & SCAUSE_INTERRUPT_BIT)
        riscv_interrupt_dispatcher(tf);
    else
        riscv_exception_dispatcher(tf); 
}

void riscv_irq_init(void) {
    // disable global interrupts
    arch_disable_interrupts();

    // set stvec with irq handler
    uintptr_t handler = (uintptr_t)riscv_trap_entry;
    asm volatile("csrw stvec, %0" : : "r"(handler));
    
    // initialize timer
    int64_t ret = riscv_timer_init(TIMEBASE_FREQUENCY);
    if (ret < 0) {
        log_error("failed to initialize timer\n");
        for (;;) {
            arch_halt();
        }
    }
    // initialize PLIC
    riscv_plic_init();

    arch_enable_interrupts();
}
