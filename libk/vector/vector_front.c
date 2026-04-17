#include "vector.h"

void* vector_front(const vector_t* vec){
    if (vector_empty(vec)){
        return NULL;
    }

    return vec->array;
}
