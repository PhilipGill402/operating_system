#include <arch/interrupts/port.h>

void arch_write_byte(uintptr_t port, uint8_t value) {
    *(volatile uint8_t*)port = value; 
    return;
}

uint8_t arch_read_byte(uintptr_t port) {
    return *(volatile uint8_t*)port;
}

uint32_t arch_read_u32(uintptr_t addr) {
    return *(volatile uint32_t*)addr;
}

void arch_write_u32(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t*)addr = value;
    return;
}

void arch_io_wait(void) {
    return;
}
