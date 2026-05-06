#include <string.h>
#include <log.h>

void strcpy(char* dst, char* src){
    if (!dst || !src) return;

    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }     
    
    *dst = '\0';
}
