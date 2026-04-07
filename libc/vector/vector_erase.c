#include "vector.h"

void erase(vector_t* vec, int index){
    if (index >= vector_size(vec)){
        return;
    }
    
    //we're removing the last element
    if (index == vector_size(vec) - 1){
        vec->size--;
    } else {
        for (int i = index; i < vector_size(vec) - 1; i++){
            memcpy((char*)vec->array + i * vec->element_size, (char*)vec->array + (i + 1) * vec->element_size, vec->element_size); 
        }

        vec->size--;
    }
    
    if (vector_size(vec) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }
}
