#include <string.h>

string_t string_literal(const char* str) {
    string_t string;
    string.len = strlen(str);
    string.capacity = string.len * 2;
    string.str = malloc(sizeof(char) * string.capacity); // this needs to be allocated on the heap so that when we release the string we can just call 'free()'
    strcpy(string.str, str);

    return string;
}
