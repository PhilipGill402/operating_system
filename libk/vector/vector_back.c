#include <vector.h>

void* vector_back(const vector_t* vec){
    if (vec->size <= 0){
        return NULL;
    }

    return (char*)vec->array + vec->size * vec->element_size;
}
