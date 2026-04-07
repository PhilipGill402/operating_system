#include <vector.h>

void* vector_next(vector_iterator_t* itr){
    if (vector_has_next(itr)){
        void* element = itr->current;
        itr->current = (char*)itr->current + itr->element_size;

        return element;
    } else {
        return NULL;
    }
}
