#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include "vga.h"
#include "idt.h"

void kernel_main(void) {
    terminal_initialize();
    idt_install();

    printf("Hello world\n"); 
}
