#include <string.h>

int strlen(const char* s){
    if (!s)
        return 0;

    int len = 0;
    while (*s != '\0'){
        s++;
        len++;
    }

    return len;
}
