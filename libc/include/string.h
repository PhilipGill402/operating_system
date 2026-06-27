#ifndef STRING_H_
#define STRING_H_

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdlib.h>
#include <vector.h>

typedef struct {
    char* str;
    size_t len;
    size_t capacity;
} string_t;

/* --- HEAP STRING HELPERS --- */
string_t string_create();
string_t string_literal(const char* str);
void string_free(string_t* str);
string_t string_clone(string_t* str);
char* string_to_literal(string_t* str);
void string_clear(string_t* str);
int string_resize(string_t* str, int capacity);

/* --- STRLEN --- */
#define strlen(str) _Generic((str),       \
    string_t*:      h_strlen,             \
    char*:          c_strlen,             \
    const char*:    c_strlen              \
)(str)

int h_strlen(string_t* string);
int c_strlen(const char*);

/* --- STRCAT --- */
#define strcat(dst, src) _Generic((dst),        \
    string_t*: _Generic((src),                  \
                string_t*: h_strcat_hstr,       \
                char: h_strcat_c,               \
                char*: h_strcat_cstr,           \
                const char*: h_strcat_cstr      \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strcat_hstr,       \
                char*: c_strcat_cstr,           \
                const char*: c_strcat_cstr,     \
                char: c_strcat_c                \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strcat_hstr,       \
                char*: c_strcat_cstr,           \
                const char*: c_strcat_cstr,     \
                char: c_strcat_c                \
                )                               \
)(dst, src)

void h_strcat_c(string_t* string, char ch);
void h_strcat_cstr(string_t* dst, const char* src);
void h_strcat_hstr(string_t* dst, string_t* src);
void c_strcat_cstr(char* dst, char* src);
void c_strcat_hstr(char* dst, string_t* src);
void c_strcat_c(char* dst, char c);

/* --- STRCMP --- */
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

int h_strcmp_hstr(string_t* a, string_t* b);
int h_strcmp_cstr(string_t* a, const char* b);
int c_strcmp_cstr(const char*, const char*);
int c_strcmp_hstr(const char*, string_t*);

/* --- STRCPY --- */
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

void h_strcpy_hstr(string_t* dst, string_t* src);
void h_strcpy_cstr(string_t* dst, const char* src);
void c_strcpy_cstr(char*, const char*);
void c_strcpy_hstr(char*, string_t*);

/* --- STRTOK --- */
#define strtok(str, delimiter) _Generic((str),  \
    string_t*:     h_strtok,                    \
    char*:          c_strtok,                   \
    const char*:    c_strtok                    \
)(str, delimiter)

vector_t h_strtok(string_t* str, char a);
vector_t c_strtok(char*, char);

/* --- STRNCMP --- */
#define strncmp(dst, src, len) _Generic((dst),  \
    string_t*: _Generic((src),                  \
                string_t*: h_strncmp_hstr,      \
                char*: h_strncmp_cstr,          \
                const char*: h_strncmp_cstr     \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strncmp_hstr,      \
                char*: c_strncmp_cstr,          \
                const char*: c_strncmp_cstr     \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strncmp_hstr,      \
                char*: c_strncmp_cstr,          \
                const char*: c_strncmp_cstr     \
                )                               \
)(dst, src, len)

int c_strncmp_cstr(const char* a, const char* b, size_t len);
int c_strncmp_hstr(const char* a, string_t* b, size_t len);
int h_strncmp_hstr(string_t* a, string_t* b, size_t len);
int h_strncmp_cstr(string_t* a, const char* b, size_t len);

/* --- STRNCPY --- */
#define strncpy(dst, src, len) _Generic((dst),  \
    string_t*: _Generic((src),                  \
                string_t*: h_strncpy_hstr,      \
                char*: h_strncpy_cstr,          \
                const char*: h_strncpy_cstr     \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strncpy_hstr,      \
                char*: c_strncpy_cstr,          \
                const char*: c_strncpy_cstr     \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strncpy_hstr,      \
                char*: c_strncpy_cstr,          \
                const char*: c_strncpy_cstr     \
                )                               \
)(dst, src, len)

void c_strncpy_cstr(char* dst, char* src, size_t len);
void c_strncpy_hstr(char* dst, string_t* src, size_t len);
void h_strncpy_hstr(string_t* dst, string_t* src, size_t len);
void h_strncpy_cstr(string_t* dst, char* src, size_t len);

/* --- STRNCAT --- */
#define strncat(dst, src, len) _Generic((dst),        \
    string_t*: _Generic((src),                  \
                string_t*: h_strncat_hstr,       \
                char: h_strncat_c                \
                char*: h_strncat_cstr,           \
                const char*: h_strncat_cstr      \
                ),                              \
    char*: _Generic((src),                      \
                string_t*: c_strncat_hstr,       \
                char*: c_strncat_cstr,           \
                const char*: c_strncat_cstr      \
            ),                                  \
    const char*: _Generic((src),                \
                string_t*: c_strncat_hstr,       \
                char*: c_strncat_cstr,           \
                const char*: c_strncat_cstr      \
                )                               \
)(dst, src, len)

void c_strncat_cstr(char* dst, char* src, size_t len);
void c_strncat_hstr(char* dst, string_t* src, size_t len);
void h_strncat_hstr(string_t* dst, string_t* src, size_t len);
void h_strncat_cstr(string_t* dst, char* src, size_t len);

/* --- STRCHAR --- */
char* c_strchar(char*, int);

/* --- STRRCHAR --- */
char* c_strrchar(char*, int);

/* --- STRSTR --- */
char* c_strstr(char*, char*);


#endif // !STRING_H_
