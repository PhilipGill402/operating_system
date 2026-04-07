#include "vector.h"

void* vector_get(const vector_t* vec, int index){
    if (index > vector_size(vec)){
        return NULL;
    }

    return (char*)vec->array + index * vec->element_size;
}
