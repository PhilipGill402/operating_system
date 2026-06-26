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

#define strlen(str) _Generic((str),       \
    string_t*:      h_strlen,             \
    char*:          c_strlen,             \
    const char*:    c_strlen              \
)(str)

#define strcat(dst, src) _Generic((dst),        \
    string_t*: _Generic((src),                  \
                string_t*: h_strcat_hstr,       \
                char: h_strcat_c                \
                char*: h_strcat_cstr,           \
                const char*: h_strcat_cstr      \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strcat_hstr,       \
                char*: c_strcat_cstr,           \
                const char*: c_strcat_cstr      \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strcat_hstr,       \
                char*: c_strcat_cstr,           \
                const char*: c_strcat_cstr      \
                )                               \
)(dst, src)

#define strcmp(dst, src) _Generic((dst),        \
    string_t*: _Generic((src),                  \
                string_t*: h_strcmp_hstr,       \
                char*: h_strcmp_cstr,           \
                const char*: h_strcmp_cstr      \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strcmp_hstr,       \
                char*: c_strcmp_cstr,           \
                const char*: c_strcmp_cstr      \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strcmp_hstr,       \
                char*: c_strcmp_cstr,           \
                const char*: c_strcmp_cstr      \
                )                               \
)(dst, src)

#define strcpy(dst, src) _Generic((dst),        \
    string_t*: _Generic((src),                  \
                string_t*: h_strcpy_hstr,       \
                char*: h_strcpy_cstr,           \
                const char*: h_strcpy_cstr      \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strcpy_hstr,       \
                char*: c_strcpy_cstr,           \
                const char*: c_strcpy_cstr      \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strcpy_hstr,       \
                char*: c_strcpy_cstr,           \
                const char*: c_strcpy_cstr      \
                )                               \
)(dst, src)

string_t string_create();
string_t string_literal(const char* str);
void string_free(string_t* str);
string_t string_clone(string_t* str);
char* string_to_literal(string_t* str);

void string_clear(string_t* str);

int h_strlen(string_t* string);

void h_strcat_c(string_t* string, char ch);
void h_strcat_cstr(string_t* dst, char* src);
void h_strcat_hstr(string_t* dst, string_t* src);

int h_strcmp_hstr(string_t* a, string_t* b);
int h_strcmp_cstr(string_t* a, const char* b);

void h_strcpy_hstr(string_t* dst, string_t* src);
void h_strcpy_cstr(string_t* dst, const char* src);

vector_t h_strtok(string_t* str, char a);

int c_strlen(const char*);

void c_strcat_cstr(char*, char*);
void c_strcat_hstr(char*, string_t*);

int c_strcmp_cstr(const char*, const char*);
int c_strcmp_hstr(const char*, string_t*);

void c_strcpy_cstr(char*, const char*);
void c_strcpy_hstr(char*, string_t*);

int c_strncmp(const char*, const char*, size_t);
void c_strncpy(char*, char*, size_t);
void c_strncat(char*, char*, size_t);
char* c_strchar(char*, int);
char* c_strrchar(char*, int);
char* c_strstr(char*, char*);
vector_t c_strtok(char*, int);

#endif // !STRING_H_
