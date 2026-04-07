#include <vector.h>

void vector_push_back(vector_t* vec, void* element) {
    // if the array is full then double its capacity  
    if (vector_size(vec) == vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    } 
    
    memcpy((char*)vec->array + vec->size * vec->element_size, element, vec->element_size);
    vec->size++;
}
