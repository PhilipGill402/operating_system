#include "io.h"
#include "utils.h"

#define CHARS_PER_LINE 80
#define MAX_LINES 25

//global so all functions can change it
int cur_x, cur_y = 0;

//moves the cursor to the current x and y position
void move_cursor() {
    unsigned temp = (cur_y * 80) + cur_x; 

    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

//writes a character to vga output
void put_char(unsigned char c) {
    short* vga = (short*)0xb8000;
    short color = 0x0F00;
    
    // position = (y * 80) + x;
    int position = (cur_y * 80) + cur_x;

    //edge cases
    if (c == 0x08 && cur_x != 0) { //backspace
        cur_x--; 
    } else if (c == 0x09) { //tab
        cur_x = (cur_x + 8) & ~(8 - 1);
    } else if (c == '\r') { //carriage return
        cur_x = 0;
    } else if (c == '\n') { //new line
        cur_x = 0;
        cur_y++;
    } else if (c >= ' ') { //only characters after the space are printable
        vga[position] = c | color;

        cur_x++;
        if (cur_x > CHARS_PER_LINE) {
            cur_x = 0;
            cur_y++;
        }
    }

    move_cursor();
}

//prints a null terminated string to vga output
void print(const char* str) {
    while (*str != '\0') {
        put_char(*str);
        str++;
    }
}
