#include <arch/interrupts/port.h>

void arch_write_byte(uintptr_t port, uint8_t value) {
    *(volatile uint8_t*)port = value; 
    return;
}

uint8_t arch_read_byte(uintptr_t port) {
    return *(volatile uint8_t*)port;
}

void arch_io_wait(void) {
    return;
}
