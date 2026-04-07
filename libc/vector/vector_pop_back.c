#include <vector.h>

void* vector_pop_back(vector_t* vec){
    if (vector_size(vec) == 0){
        return NULL;
    } 

    //if the size of the vector drops to 25% of its capacity then halve its capacity
    if ((vector_size(vec) - 1) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }

    vec->size--;
    void* element = (char*)vec->array + vec->size * vec->element_size;

    return element;
}
