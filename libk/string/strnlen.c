#include <string.h>

int strnlen(const char* s, size_t maxlen){
    if (!s)
        return 0;

    int len = 0;
    while (*s != '\0' && len < maxlen){
        s++;
        len++;
    }

    return len;
}
