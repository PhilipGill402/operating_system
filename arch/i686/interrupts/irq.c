#include <arch/interrupts/irq.h>

void irq_install_handler(uint8_t irq, irq_handler_t handler) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(uint8_t irq) {
    irq_routines[irq] = 0;
}

void irq_handler(regs_t* r) {
    uint32_t irq = r->int_no - 32;
    
    if (irq < 16 && irq_routines[irq]) {
        irq_routines[irq](r);
    }

    pic_send_eoi(irq); 
}

void irq_init_handlers() {
    irq_install_handler(0, timer_callback);
    irq_install_handler(1, keyboard_callback);
}
