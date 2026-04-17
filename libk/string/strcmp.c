#include <string.h>

int strcmp(const char* s1, const char* s2){
    int ascii1 = (int)*s1;
    int ascii2 = (int)*s2;

    while (ascii1 - ascii2 == 0 && *s1 != '\0' && *s2 != '\0'){
        s1++;
        s2++;
        ascii1 = (int)*s1;
        ascii2 = (int)*s2;
    }

    return ascii1 - ascii2;
}
