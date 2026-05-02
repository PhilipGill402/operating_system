#include <stdio.h>

int puts(const char* str) {
    return kvprintf(terminal_putchar, "%s\n", str);
}
