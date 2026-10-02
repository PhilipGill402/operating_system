#include <arch/interrupts/port.h>

void arch_write_byte(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t arch_read_byte(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void arch_io_wait(void) {
    __asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
}
