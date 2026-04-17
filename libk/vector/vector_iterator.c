#include <vector.h>

vector_iterator_t* vector_iterator(vector_t* vec) {
    vector_iterator_t* itr;
    itr = kmalloc(sizeof(*itr));
    itr->current = vec->array;
    itr->end = vec->array + vector_size(vec) * vec->element_size;
    itr->element_size = vec->element_size;
    return itr;
}
