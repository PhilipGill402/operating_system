#ifndef STRING_H_
#define STRING_H_

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdlib.h>
#include <vector.h>

typedef struct {
    char* str;
    int len;
    int capacity;
} string_t;

string_t string_create();
string_t string_literal(const char* str);
int string_len(string_t* string);
void string_copy(string_t* dst, string_t* src);
void string_append_chr(string_t* string, char ch);
void string_cat(string_t* dst, string_t* src);
int string_compare(string_t* a, string_t* b);
void string_clear(string_t* str);
int string_compare_literal(string_t* a, const char* b);
void string_free(string_t* str);
string_t string_clone(string_t* str);
vector_t string_tokenize(string_t* str, char a);
char* string_to_literal(string_t* str);

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
