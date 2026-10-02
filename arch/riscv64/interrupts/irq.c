#include <arch/interrupts/irq.h>

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
