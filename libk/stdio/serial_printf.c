#include <stdio.h>

int serial_printf(const char* __restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = kvprintf(serial_write_char, NULL, fmt, args);
    va_end(args);

    return written;
}
