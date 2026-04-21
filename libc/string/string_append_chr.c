#include <string.h>


void string_append_chr(string_t* string, char ch) {
    if (string->len + 1 > string->capacity) {
        string->str = realloc(string->str, sizeof(char) * 2 * (string->len + 1));

        if (!string->str) {
            printf("realloc failed");
            return;
        }

        string->capacity = 2 * (string->len + 1);
    }

    string->str[string->len] = ch;
    string->len++;
}
