#include <string.h>

int strncmp(const char* s1, const char* s2, size_t n){
    int ascii1 = (int)*s1;
    int ascii2 = (int)*s2;
    size_t idx = 1;

    while (ascii1 - ascii2 == 0 && *s1 != '\0' && *s2 != '\0' && idx < n){
        s1++;
        s2++;
        ascii1 = (int)*s1;
        ascii2 = (int)*s2;
        idx++;
    }

    return ascii1 - ascii2;
}
