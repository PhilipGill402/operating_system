#include <vector.h>

vector_iterator_t* vector_iterator(vector_t* vec) {
    vector_iterator_t* itr;
    itr = malloc(sizeof(*itr));
    itr->current = vec->array;
    itr->end = vec->array + vector_size(vec) * vec->element_size;
    itr->element_size = vec->element_size;
    return itr;
}

int vector_has_next(const vector_iterator_t* itr){
    return itr->current != itr->end;
}

void* vector_next(vector_iterator_t* itr){
    if (vector_has_next(itr)){
        void* element = itr->current;
        itr->current = (char*)itr->current + itr->element_size;

        return element;
    } else {
        return NULL;
    }
}
