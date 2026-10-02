#include <arch/interrupts/idt.h>

void idt_set_gate(unsigned char num, unsigned long base, unsigned short seg, unsigned char flags) {
    return;
}

void idt_create_isr_stubs() {
    return;
}

void idt_create_irq_stubs() {
    return;
}

void idt_install() {
    return;
}

void idt_load() {
    return;
}

void isr_handler(arch_trapframe_t* tf) {
    return;
}
