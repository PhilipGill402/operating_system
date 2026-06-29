#include <string.h>

#include <stdio.h>

void c_strcat_cstr(char* dst, char* src){
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }
}

void c_strcat_hstr(char* dst, string_t* src) {
    // assume destination has enough space 
    char* p = dst + c_strlen(dst); 

    for (uint32_t i = 0; i < src->len; i++) {
        *p = src->str[i];
        p++;
    }

    *p = '\0';
}

void c_strcat_c(char* dst, char c) {
    while (*dst)
        dst++;

    *dst = c;
    dst++;
    *dst = '\0';
}

char* c_strchar(char* str, int c){
    char ch = (char)c;
    uint32_t len = c_strlen(str);

    for (uint32_t i = 0; i <= len; i++) {
        if (str[i] == ch)
            return &str[i];
    }

    return NULL;
}

int c_strcmp_cstr(const char* s1, const char* s2){
    for (uint32_t i = 0; s1[i] != '\0' && s2[i] != '\0'; i++) {
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
    }

    return 0;
}

int c_strcmp_hstr(const char* s1, string_t* s2) {
    for (uint32_t i = 0; i < s2->len && s1[i] != '\0'; i++) {
        if (s2->str[i] != s1[i])
            return s1[i] - s2->str[i];
    }

    return 0;
}

uint32_t c_strlen(const char* s){
    uint32_t len = 0;
    while (*s != '\0'){
        s++;
        len++;
    }

    return len;
}

void c_strncat_cstr(char* dst, char* src, size_t n){
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }

    *dst = '\0';
}

void c_strncat_hstr(char* dst, string_t* src, size_t len) {
    while (*dst != '\0')
        dst++;

    for (uint32_t i = 0; i < src->len && i < len; i++) {
        *dst = src->str[i];
        dst++;
    }

    *dst = '\0';
}

int c_strncmp_cstr(const char* a, const char* b, size_t n){
    for (uint32_t idx = 0; a[idx] != '\0' && b[idx] != '\0' && idx < n; idx++) {
        int diff = (int)a[idx] - (int)b[idx];

        if (diff != 0)
            return diff;
    } 

    return 0;
}

int c_strncmp_hstr(const char* a, string_t* b, size_t len) {
    for (uint32_t idx = 0; idx < b->len && a[idx] != '\0' && idx < len; idx++) {
        int diff = (int)a[idx] - (int)b->str[idx];

        if (diff != 0)
            return diff;
    }

    return 0;
}

void c_strncpy_cstr(char* dst, char* src, size_t n){
    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
}

void c_strncpy_hstr(char* dst, string_t* src, size_t n) {
    for (uint32_t i = 0; i < src->len && i < n; i++) {
        *dst = src->str[i];
        dst++;
    }
    *dst = '\0';
}

char* c_strrchar(char* str, int c){
    char ch = (char)c;
    uint32_t len = c_strlen(str);
    char* c_ptr = NULL;

    for (uint32_t i = 0; i <= len; i++) {
        if (str[i] == ch)
            c_ptr = &str[i];
    }

    return c_ptr;
}

char* c_strstr(char* str, char* substr){
    if (substr[0] == '\0')
        return str;

    char* found = NULL;
    char* substr_reset = substr;
    while (*str != '\0'){
        if (*str == *substr){
            char* secondptr = str;
            while (*secondptr != '\0' && *secondptr == *substr){
                secondptr++;
                substr++;
            }
            if (*substr == '\0'){
                found = str;
                break;
            } else {
                substr = substr_reset;
            }
        }
        str++;
    }
     
    return found;
}

vector_t c_strtok(char* str, char delimiter){
    vector_t tokens = vector_create(sizeof(char *));

    if (str == NULL) {
        return tokens;
    }

    int str_len = c_strlen(str);

    while (*str != '\0') {
        while (*str == delimiter)
            str++;

        if (*str == '\0')
            break;

        char* tok = malloc(str_len + 1);
        if (tok == NULL)
            return tokens;

        int tok_idx = 0;

        while (*str != '\0' && *str != delimiter) {
            tok[tok_idx++] = *str;
            str++;
        }

        tok[tok_idx] = '\0';
        vector_push_back(&tokens, &tok);
    }

    return tokens;

}

void c_strcpy_cstr(char* dst, const char* src){
    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }     
    
    *dst = '\0';
}

void c_strcpy_hstr(char* dst, string_t* src) {
    for (uint32_t i = 0; i < src->len; i++) {
        *dst = src->str[i];
        dst++;
    }

    *dst = '\0';
}


