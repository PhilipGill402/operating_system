#ifndef STDIO_H_
#define STDIO_H_


#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <../../include/io/serial.h>
#include <../../include/io/vga.h>

#define EOF (-1)

int serial_printf(const char* __restrict, ...);
int terminal_printf(const char* __restrict, ...);
int kvprintf(void (*putc)(char c), const char* fmt, va_list args);
int putchar(int);
int puts(const char*);

#endif // !STDIO_H_
