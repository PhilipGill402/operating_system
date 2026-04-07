#include <vector.h>

void shrink_to_fit(vector_t* vec){
    vec->capacity = vec->size;
}
