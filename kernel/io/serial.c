#include "io/serial.h"

static int serial_transmit_empty() {
    return arch_read_byte(COM1 + 5) & 0x20;
}

void serial_write_char(char c, void* ctx) {
    (void)ctx;    

    while (!serial_transmit_empty()) {}

    if (c == '\n') {
        arch_write_byte(COM1, '\r');
        while (!serial_transmit_empty());
    }

    arch_write_byte(COM1, c);
}
void serial_write(const char* str) {
    while (*str) {
        serial_write_char(*str, NULL);
        str++;
    }
}

void serial_init() {
    arch_write_byte(COM1 + 1, 0x00); // Disable interrupts
    arch_write_byte(COM1 + 3, 0x80); // Enable DLAB
    arch_write_byte(COM1 + 0, 0x03); // Divisor low byte: 38400 baud
    arch_write_byte(COM1 + 1, 0x00); // Divisor high byte
    arch_write_byte(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    arch_write_byte(COM1 + 2, 0xC7); // Enable FIFO, clear them, 14-byte threshold
    arch_write_byte(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

