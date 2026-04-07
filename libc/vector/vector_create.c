#include <vector.h>

vector_t vector_create(size_t element_size){
    //start with 10 elements
    vector_t vec;
    vec.array = malloc(10 * element_size);

    if (!vec.array){
        vec.size = 0;
        vec.capacity = 0;
        
        //returns zeroed out array in case of error
        return vec;
    }

    vec.size = 0;
    vec.capacity = 10;
    vec.element_size = element_size;

    return vec;
}
