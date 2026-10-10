#include <stdlib.h>
#include <stdio.h>
#include <arch/asm/helpers.h>
#include <log.h>

void panic(char* msg) {
    serial_printf("KERNEL PANIC: %s", msg);

    for (;;) {
        arch_halt();
    }
}
