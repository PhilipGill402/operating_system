#ifndef STRING_H_
#define STRING_H_

#include <sys/cdefs.h>
#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

typedef struct {
    char* str;
    int len;
    int capacity;
} string_t;

string_t create_string();
string_t string_literal(const char* str);
int string_len(string_t* string);
void string_copy(string_t* dst, string_t* src);
void string_append_chr(string_t* string, char ch);
void string_cat(string_t* dst, string_t* src);
int string_compare(string_t* a, string_t* b);
void string_free(string_t* str);

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

int strlen(const char*);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);
void strcpy(char*, char*);
void strncpy(char*, char*, size_t);
void strcat(char*, char*);
void strncat(char*, char*, size_t);
char* strchar(char*, int);
char* strrchar(char*, int);
char* strstr(char*, char*);
char* strtok(char*, int);

#endif // !STRING_H_
