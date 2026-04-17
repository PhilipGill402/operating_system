#include <vector.h>

bool vector_empty(const vector_t* vec){
    if (vec == NULL) {
        return true;
    }
    
    return vector_size(vec) == 0;
}
