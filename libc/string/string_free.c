#include <string.h>

void string_free(string_t* str) {
    free(str->str);

    str->str = NULL;
    str->len = 0;
    str->capacity = 0;
}
