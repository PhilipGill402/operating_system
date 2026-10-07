#include "interrupts/irq.h"
#include "trapframe.h"
#include <arch/asm/helpers.h>
#include <stdio.h>
#include <stdint.h>

extern void riscv_trap_entry(void);

void riscv_trap_handler(arch_trapframe_t* tf) {
    serial_printf("scause: %l\n", tf->scause);
    serial_printf("sepc: %l\n", tf->sepc);
    serial_printf("stval: %l\n", tf->stval);
    serial_printf("sstatus: %l\n", tf->sstatus);

    for (;;) {
        arch_halt();
    }
}

void riscv_irq_init(void) {
    // disable global interrupts
    arch_disable_interrupts();
    // set stvec with irq handler
    uintptr_t handler = (uintptr_t)riscv_trap_entry;
    asm volatile("csrw stvec, %0" : : "r"(handler));
    // enable timer interrupts (enable others after timer works)
    uint64_t mask = 1ULL << 5;
    asm volatile("csrs sie, %0" : : "r"(mask));
    // initialize PLIC
}
