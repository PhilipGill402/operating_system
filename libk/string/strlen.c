#include <string.h>

int strlen(const char* s){
    int len = 0;
    while (*s != '\0'){
        s++;
        len++;
    }

    return len;
}
