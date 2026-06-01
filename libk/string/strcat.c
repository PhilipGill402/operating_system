#include <string.h>

char* strcat(char* dst, char* src){
    char* ret = dst; 
    
    while (*dst != '\0'){
        dst++;
    }

    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }

    *dst = '\0';

    return ret;
}
