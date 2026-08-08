#include <arch/interrupts/port.h>

void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    arch_write_byte(0x43, 0x36);
    arch_write_byte(0x40, divisor & 0xFF);
    arch_write_byte(0x40, (divisor >> 8) & 0xFF);
}
