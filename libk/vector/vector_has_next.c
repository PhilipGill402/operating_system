#include "vector.h"

int vector_has_next(const vector_iterator_t* itr){
    return itr->current != itr->end;
}
