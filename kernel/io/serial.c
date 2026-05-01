#include "io/serial.h"

static int serial_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write_char(char c) {
    while (!serial_transmit_empty()) {}

    if (c == '\n') {
        outb(COM1, '\r');
        while (!serial_transmit_empty());
    }

    outb(COM1, c);
}
void serial_write(const char* str) {
    while (*str) {
        serial_write_char(*str);
        str++;
    }
}

void serial_init() {
    outb(COM1 + 1, 0x00); // Disable interrupts
    outb(COM1 + 3, 0x80); // Enable DLAB
    outb(COM1 + 0, 0x03); // Divisor low byte: 38400 baud
    outb(COM1 + 1, 0x00); // Divisor high byte
    outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, 14-byte threshold
    outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

