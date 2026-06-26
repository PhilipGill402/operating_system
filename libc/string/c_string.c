#include <string.h>

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
    char* p = dst[c_strlen(dst)]; 

    for (uint32_t i = 0; i < src->len) {
        *p = src->str[i];
        p++;
    }

    *p = '\0';
}

char* c_strchar(char* str, int c){
    char ch = (char)c;
    char* c_ptr = NULL; 

    while (*str != ch && *str != '\0'){
        str++;
    }

    if (*str == ch){
        c_ptr = str;
    }
    
    return c_ptr;
    
}

int c_strcmp_cstr(const char* s1, const char* s2){
    int ascii1 = (int)*s1;
    int ascii2 = (int)*s2;

    while (ascii1 - ascii2 == 0 && *s1 != '\0' && *s2 != '\0'){
        s1++;
        s2++;
        ascii1 = (int)*s1;
        ascii2 = (int)*s2;
    }

    return ascii1 - ascii2;
}

int c_strcmp_hstr(const char* s1, string_t* s2) {
    for (uint32_t i = 0; i < a->len && b[i] != '\0'; i++) {
        if (a->str[i] != b[i]) {
            return a->str[i] - b[i];
        }
    }

    return 0;
}

int c_strlen(const char* s){
    int len = 0;
    while (*s != '\0'){
        s++;
        len++;
    }

    return len;
}

void c_strncat(char* dst, char* src, size_t n){
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
}

int c_strncmp(const char* s1, const char* s2, size_t n){
    int ascii1 = (int)*s1;
    int ascii2 = (int)*s2;
    size_t idx = 1;

    while (ascii1 - ascii2 == 0 && *s1 != '\0' && *s2 != '\0' && idx < n){
        s1++;
        s2++;
        ascii1 = (int)*s1;
        ascii2 = (int)*s2;
        idx++;
    }

    return ascii1 - ascii2;
}

void c_strncpy(char* dst, char* src, size_t n){
    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
}

char* c_strrchar(char* str, int c){
    char ch = (char)c;
    char* c_ptr = NULL;

    while (*str != '\0'){
        if (*str == ch){
            c_ptr = str;
        }
        str++;
    }

    return c_ptr;
}


char* c_strstr(char* str, char* substr){
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

char* c_strtok(char* str, int delimiter){
    static char* string = NULL;
    if (str != NULL) {
        string = str;
    }
    
    if (string == NULL) {
        return NULL;
    }
    
    while (*string != '\0' && *string == (char)delimiter){
        string++;
    }
    
    if (*string == '\0') {
        string = NULL; 
        return NULL;
    }

    char* start = string;
    
    while (*string != '\0' && *string != (char)delimiter){
        string++;
    }

    if (*string) {
        *string = '\0';
        string++;
    } else {
        string = NULL;
    }
    
    return start;
}

void c_strcpy_cstr(char* dst, char* src){
    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }     
    
    *dst = '\0';
}

void c_str_cpy_hstr(char* dst, string_t* src) {
    for (uint32_t i = 0; i < src->len; i++) {
        *dst = src->str[i];
        dst++;
    }

    *dst = '\0';
}


