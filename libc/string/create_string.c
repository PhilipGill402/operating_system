#include <string.h>

string_t create_string() {
    string_t string;

    string.str = malloc(sizeof(char) * 10);
    string.capacity = 10;
    string.len = 0;

    return string;
}
