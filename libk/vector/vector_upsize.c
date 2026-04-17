#include <vector.h>

void upsize(vector_t* vec, int new_size, void* element){
    if (new_size < vector_size(vec)){
        return;
    } 
    
    int old_size = vec->size; 
    vec->size = new_size;

    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }

    for (int i = old_size; i < new_size; i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}
