#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include "vga.h"
#include "idt.h"
#include "gdt.h"

void kernel_main(void) {
    terminal_initialize();
    gdt_install();
    idt_install();

    printf("Hello world\n");

    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;
}
