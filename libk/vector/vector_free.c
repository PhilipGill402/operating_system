#include "vector.h"

void vector_free(vector_t* vec){
    kfree(vec->array);
    vec->array = NULL;
    vec->size = 0;
    vec->capacity = 0;
}
