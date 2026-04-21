#include <string.h>

int string_compare(string_t* a, string_t* b) { 
    for (int i = 0; i < a->len && i < b->len; i++) {
        if (a->str[i] != b->str[i]) {
            return a->str[i] - b->str[i];
        }
    } 

    return 0;
}
