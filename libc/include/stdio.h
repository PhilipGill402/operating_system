#ifndef STDIO_H_
#define STDIO_H_

#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define EOF (-1)
#define stdout 1
#define stdin 1
#define stderr 1

#define sprintf(str, fmt, ...) _Generic((str),  \
    string_t*: sprintf_s,                       \
    char*: sprintf_c                            \
)(str, fmt, ##__VA_ARGS__)

#define snprintf(str, size, fmt, ...) _Generic((str),   \
    string_t*: snprintf_s,                              \
    char*: snprintf_c                                   \
)(str, size, fmt, ##__VA_ARGS__)

int printf(const char* fmt, ...);
int kvprintf(void (*putc)(char c, void* ctx), void* ctx, const char* format, va_list args);
int sprintf_c(char* str, const char* fmt, ...);
int sprintf_s(string_t* str, const char* fmt, ...);
int snprintf_c(char* str, size_t size, const char* fmt, ...);
int snprintf_s(string_t* str, size_t size, const char* fmt, ...);

int putchar(int c);
int puts(const char* str);
int getchar();

#endif // !STDIO_H_
