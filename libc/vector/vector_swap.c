#include <vector.h>

void vector_swap(vector_t* vec, vector_t* other_vec){
    //array 
    void* tmp = vec->array; 
    vec->array = other_vec->array;
    other_vec->array = tmp;
    
    //size
    int temp = vec->size;
    vec->size = other_vec->size;
    other_vec->size = temp;

    //capcaity
    temp = vec->capacity;
    vec->capacity = other_vec->capacity;
    other_vec->capacity = temp;

}
