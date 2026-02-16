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

//scrolls the screen down
void scroll() {
    //will hold the characters in each line while copying
    unsigned temp[CHARS_PER_LINE];
    short* vga = (short*)0xb8000;
    
    //clear first line
    for (int i = 0; i < CHARS_PER_LINE; i++) {
        vga[i] = ' ';
    }

    for (int i = 1; i < MAX_LINES-1; i++) {
        for (int j = 0; j < CHARS_PER_LINE; j++) {
            int curr_pos = (i * 80) + j;
            int new_pos = ((i - 1) * 80) + j;
            vga[new_pos] = vga[curr_pos];
        }
    }
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

        if (cur_y > MAX_LINES) {
            scroll();
            cur_y--;
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

void printi(int num) {
    //TODO: replace with (int)((ceil(log10(num))+1)*sizeof(char)) to guarantee that the array is large enough
    char str[30];
    int_to_str(num, str);
    print(str);
}

void int_to_str(int num, char* str) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int sign = num;

    if (num < 0) {
        num *= -1;
    }
    
    int i = 0;
    while (num > 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }

    if (sign < 0) {
        str[i++] = '-';
    }

    str[i] = '\0';
    
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void clear_scr(){
    short* vga = (short*)0xb8000;

    for (int i = 0; i < MAX_LINES; i++) {
        for (int j = 0; j < CHARS_PER_LINE; j++) {
            int position = (i * 80) + j;
            vga[position] = ' ';
        }
    }

    cur_x = 0;
    cur_y = 0;
}
