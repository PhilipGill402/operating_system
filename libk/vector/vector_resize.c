#include <vector.h>

void vector_resize(vector_t* vec, int new_size){
    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * new_size);
    } 
    
    vec->size = new_size;
}
