#include "drivers/uart.h"
#include "io/input_buffer.h"
#include "memory/mmio.h"
#include <arch/interrupts/port.h>
#include <stddef.h>

void uart_init(void) {
    arch_write_byte(UART_ADDR + 1, 0x00); // Disable interrupts
    arch_write_byte(UART_ADDR + 3, 0x80); // Enable DLAB
    arch_write_byte(UART_ADDR + 0, 0x03); // Divisor low byte: 38400 baud
    arch_write_byte(UART_ADDR + 1, 0x00); // Divisor high byte
    arch_write_byte(UART_ADDR + 3, 0x03); // 8 bits, no parity, one stop bit
    arch_write_byte(UART_ADDR + 1, 0x01); // enable received-data-available interrupt 
    arch_write_byte(UART_ADDR + 2, 0xC7); // Enable FIFO, clear them, 14-byte threshold
    arch_write_byte(UART_ADDR + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static int uart_transmit_empty() {
    return arch_read_byte(UART_ADDR + 5) & 0x20;
}

void uart_write_char(char c, void* ctx) {
    (void)ctx;    

    if (c == '\n') {
        while (!uart_transmit_empty());
        arch_write_byte(UART_ADDR, '\r');

        while (!uart_transmit_empty());
        arch_write_byte(UART_ADDR, '\n');

        return;
    }

    while (!uart_transmit_empty());
    arch_write_byte(UART_ADDR, c);
}

void uart_handler(arch_trapframe_t* tf) {
    char c = (char)arch_read_byte(UART_ADDR + 0);
    input_buffer_write_char(c);
}


