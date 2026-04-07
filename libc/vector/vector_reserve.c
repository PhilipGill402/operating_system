#include <vector.h>

void vector_reserve(vector_t* vec, int new_capacity) {
    void* ptr = realloc(vec->array, vec->element_size * new_capacity);
    vec->array = ptr;

    if (vec->array == NULL){
        return;
    }
    
    vec->capacity = new_capacity;
}
