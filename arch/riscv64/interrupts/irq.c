#include "interrupts/irq.h"

extern void riscv_trap_entry(void);

void riscv_irq_init(void) {
    // disable global interrupts
    arch_disable_interrupts();
    // set stvec with irq handler
    uintptr_t handler = riscv_trap_entry;
    asm volatile("csrw stvec, %0" : : "r"(handler));
    // enable timer interrupts (enable others after timer works)
    // initialize PLIC
}
