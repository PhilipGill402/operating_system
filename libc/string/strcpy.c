#include <string.h>

void strcpy(char* dst, char* src){
    while (*src != '\0'){
        *dst = *src;
        src++;
        dst++;
    }     
    
    *dst = '\0';
}
