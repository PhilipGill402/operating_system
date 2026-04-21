#include <string.h>
#include <stdio.h>


void string_copy(string_t* dst, string_t* src) {
    if (src->len > dst->capacity) {
        dst->str = realloc(dst->str, sizeof(char) * 2 * src->len);
        
        if (!dst->str) {
            printf("realloc failed");
            return;
        }

        dst->capacity = 2 * src->len;
    } 

    for (int i = 0; i < src->len; i++) {
        dst->str[i] = src->str[i];
    }

    dst->len = src->len;
}
