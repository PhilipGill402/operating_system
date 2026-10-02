#include <arch/interrupts/port.h>

void arch_write_byte(uint16_t port, uint8_t value) {
    return;
}

uint8_t arch_read_byte(uint16_t port) {
    return 0;
}

void arch_io_wait(void) {
    return;
}
