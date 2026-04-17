#ifndef STDIO_H_
#define STDIO_H_

#define EOF (-1)
#define stdout 1
#define stdin 1
#define stderr 1

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

#endif // !STDIO_H_
