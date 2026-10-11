#include "io/serial.h"
#include <arch/interrupts/port.h>
#include <arch/drivers/uart.h>

void serial_write(const char* str) {
    while (*str) {
        uart_write_char(*str, NULL);
        str++;
    }
}
