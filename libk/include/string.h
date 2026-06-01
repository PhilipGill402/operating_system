#ifndef STRING_H_
#define STRING_H_

#include <sys/cdefs.h>
#include <stddef.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

int strlen(const char*);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);
void strcpy(char*, char*);
void strncpy(char*, char*, size_t);
char* strcat(char*, char*);
void strncat(char*, char*, size_t);
char* strchar(char*, int);
char* strrchar(char*, int);
char* strstr(char*, char*);
char* strtok(char*, int);

#endif // !STRING_H_
