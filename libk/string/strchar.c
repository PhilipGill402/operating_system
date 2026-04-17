#include <string.h>

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
