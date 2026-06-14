#include <string.h>

void strcat(char* dst, char* src){
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }
}

char* strchar(char* str, int c){
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

int strcmp(const char* s1, const char* s2){
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

int strlen(const char* s){
    int len = 0;
    while (*s != '\0'){
        s++;
        len++;
    }

    return len;
}

void strncat(char* dst, char* src, size_t n){
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
}

int strncmp(const char* s1, const char* s2, size_t n){
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

void strncpy(char* dst, char* src, size_t n){
    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
}

char* strrchar(char* str, int c){
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


char* strstr(char* str, char* substr){
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

char* strtok(char* str, int delimiter){
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

void strcpy(char* dst, char* src){
    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }     
    
    *dst = '\0';
}


