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
