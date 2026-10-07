#include <arch/interrupts/irq.h>
#include <arch/asm/helpers.h>

extern void trap_entry(void);

void irq_install_handler(uint8_t irq, irq_handler_t handler) {
    return;
}

void irq_uninstall_handler(uint8_t irq) {
    return;
}

void irq_handler(arch_trapframe_t* tf) {
    return;
}

void irq_init_handlers() {
    return;
}

void riscv_trap_handler(arch_trapframe_t* tf) {

}

void arch_irq_init(void) {
    // disable global interrupts
    arch_disable_interrupts();
    // set stvec with irq handler
    uintptr_t handler = riscv_trap_entry;
    asm volatile("csrw stvec, %0" : : "r"(handler));
    // enable timer interrupts (enable others after timer works)
    // initialize PLIC
}
