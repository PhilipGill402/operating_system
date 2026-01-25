#ifndef OS_IO_H_
#define OS_IO_H_

void move_cursor();
void scroll();
void put_char(unsigned char c);
void print(const char* str);
void printi(int num);
void int_to_str(int num, char* str);

#endif
