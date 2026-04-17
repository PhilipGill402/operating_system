#include <vector.h>

void vector_clear(vector_t* vec){
    vec->size = 0;
    vector_reserve(vec, 10);
}
