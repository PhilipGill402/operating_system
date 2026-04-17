#include <vector.h>

void vector_assign(vector_t* vec, void* element, int num_copies){
    if (num_copies > vec->capacity){
        vector_reserve(vec, num_copies * 2);
    }

    vec->size = num_copies;

    for (int i = 0; i < vector_size(vec); i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}
