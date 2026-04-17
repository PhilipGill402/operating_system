#include <string.h>

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
