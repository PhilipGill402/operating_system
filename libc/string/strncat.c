#include <string.h>

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
