#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vga.h"

void kernel_main(void) {
    terminal_initialize(); 
    
    for (int j = 0; j < VGA_HEIGHT; j++) {
        for (int i = 0; i < VGA_WIDTH; i++) {
            terminal_putchar('a'+j);
        }
    }

    terminal_write_string("Hello world\n");
}
