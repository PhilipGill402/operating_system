#include <string.h>
#include <stdio.h>

void string_cat(string_t* dst, string_t* src) {
    if (dst->capacity < src->len + dst->len) {
        dst->str = realloc(dst->str, sizeof(char) * 2 * (src->len + dst->len));

        if (!dst->str) {
            printf("realloc failed");
            return;
        }

        dst->capacity = 2 * (src->len + dst->len);
    }
    
    for (int i = dst->len, j = 0; i < dst->len + src->len; i++, j++) {
        dst->str[i] = src->str[j];
    }

    dst->len += src->len;
}
