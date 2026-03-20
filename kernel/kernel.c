#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include "vga.h"

void kernel_main(void) {
    terminal_initialize();

    printf("Hello world\n"); 
}
