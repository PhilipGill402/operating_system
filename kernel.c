#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xb8000

size_t strlen(const char* str) {
    size_t count = 0;

    while (*str) {
        count++;
        str++;
    }

    return count;
}

enum vga_color {
    VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
} 

void terminal_putchar_at(char c, uint8_t color, size_t x, size_t y) {
    size_t idx = y * VGA_WIDTH + x;
    terminal_buffer[idx] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    terminal_putchar_at(c, terminal_color, terminal_column, terminal_row);
    
    if (++terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row >= VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_write(const char* str, size_t size) {
    for (int i = 0; i < size; i++) {
        terminal_putchar(str[i]);
    }
}

void terminal_write_string(const char* str) {
    terminal_write(str, strlen(str));
}

void kernel_main(void) {
    terminal_write_string("Hello world\n");
}
