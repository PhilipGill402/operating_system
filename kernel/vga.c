#include "vga.h"

size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

void terminal_initialize() {
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar_at(' ', terminal_color, x, y); 
        }
    }
}

void terminal_putchar_at(char c, uint8_t color, size_t x, size_t y) {
    size_t idx = y * VGA_WIDTH + x;
    terminal_buffer[idx] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == 0x08 && terminal_row != 0) { // backspace
        terminal_column--;
    } else if (c == '\t') { // tab
        terminal_column = (terminal_column + 8) & ~(8 - 1);
    } else if (c == '\r') { // carriage return
        terminal_column = 0;
    } else if (c == '\n') { // new line
        terminal_column = 0;
        if (++terminal_row >= VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
            terminal_scroll();
        }
    } else if (c >= ' ') { // only ascii characters after ' ' are printable
        terminal_putchar_at(c, terminal_color, terminal_column, terminal_row);
    
        if (++terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row >= VGA_HEIGHT) {
                terminal_row = VGA_HEIGHT - 1;
                terminal_scroll();
            }
        }
    }

    
}

void terminal_write(const char* str, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(str[i]);
    }
}

void terminal_write_string(const char* str) {
    terminal_write(str, strlen(str));
}

void terminal_scroll() {
    // clear first line
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_putchar_at(' ', terminal_color, i, 0);
    }
    
    // move all characters in one line to the line above it
    for (int i = 1; i < VGA_HEIGHT; i++) {
        for (int j = 0; j < VGA_WIDTH; j++) {
            int curr_pos = (i * VGA_WIDTH) + j;
            int new_pos = ((i - 1) * VGA_WIDTH) + j;
            terminal_buffer[new_pos] = terminal_buffer[curr_pos];
        }
    }

    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_putchar_at(' ', terminal_color, i, VGA_HEIGHT - 1);
    }
}


