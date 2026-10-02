#include <stdlib.h>
#include <stdio.h>
#include <arch/asm/helpers.h>

__attribute__((__noreturn__))
void abort(void) {
    serial_printf("kernel: panic: abort()\n");
    while (1) {
        arch_halt();
    }

    __builtin_unreachable();
}
