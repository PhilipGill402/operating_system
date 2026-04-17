#include <vector.h>

void vector_downsize(vector_t* vec, int new_size){
    if (new_size > vector_size(vec)){
        return;
    } 
     
    vec->size = new_size;

    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }
}
