#include <vector.h>

void vector_print(const vector_t* vec, void (*print_element)(void*)) {
    printf("<"); 
    for (int i = 0; i < vector_size(vec); i++) {
        print_element(vector_get(vec, i));
        
        if (i != vector_size(vec) - 1) {
            printf(", ");
        }
    }
    printf(">\n");
}
