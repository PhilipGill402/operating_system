#include <string.h>

void strncpy(char* dst, char* src, size_t n){
    while (*src != '\0' && n-- > 0){
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
}
