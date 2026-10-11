#include <stdio.h>
#include <arch/drivers/uart.h>

int serial_printf(const char* __restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = kvprintf(uart_write_char, NULL, fmt, args);
    va_end(args);

    return written;
}
