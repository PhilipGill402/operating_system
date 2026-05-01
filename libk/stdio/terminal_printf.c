#include <stdio.h>

int terminal_printf(const char* __restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = kvprintf(terminal_putchar, fmt, args);
    va_end(args);

    return written;
}
