#include <vector.h>

void vector_insert(vector_t* vec, void* element, int index){
    //if the vector is full then reserve double its capacity
    if (vector_size(vec) + 1 > vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    }

    //shift everything over one
    for (int i = vector_size(vec) + 1; i >= index; i--){
        memcpy((char*)vec->array + i * vec->element_size, (char*)vec->array + (i - 1) * vec->element_size, vec->element_size); 
    }
    //set the element at index to the given element
    memcpy((char*)vec->array + index * vec->element_size, element, vec->element_size);
    vec->size++;
}
