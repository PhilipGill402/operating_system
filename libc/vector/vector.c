#include <vector.h>

void vector_assign(vector_t* vec, void* element, int num_copies){
    if (num_copies > vec->capacity){
        vector_reserve(vec, num_copies * 2);
    }

    vec->size = num_copies;

    for (int i = 0; i < vector_size(vec); i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}

void* vector_back(const vector_t* vec){
    if (vec->size <= 0){
        return NULL;
    }

    return (char*)vec->array + vec->size * vec->element_size;
}

int vector_capacity(const vector_t* vec){
    return vec->capacity;
}

void vector_clear(vector_t* vec){
    vec->size = 0;
    vector_reserve(vec, 10);
}

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

void* vector_data(const vector_t* vec){
    return vec->array;
}

void vector_downsize(vector_t* vec, int new_size){
    if (new_size > vector_size(vec)){
        return;
    } 
     
    vec->size = new_size;

    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }
}

bool vector_empty(const vector_t* vec){
    if (vec == NULL) {
        return true;
    }
    
    return vector_size(vec) == 0;
}

void vector_erase(vector_t* vec, int index){
    if (!vec || index < 0 || index >= vector_size(vec)){
        return;
    }
    
    //we're removing the last element
    if (index == vector_size(vec) - 1){
        vec->size--;
    } else {
        for (int i = index; i < vector_size(vec) - 1; i++){
            memmove((char*)vec->array + i * vec->element_size, (char*)vec->array + (i + 1) * vec->element_size, vec->element_size);
        }

        vec->size--;
    }
    
    if (vector_size(vec) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }
}

void* vector_front(const vector_t* vec){
    if (vector_empty(vec)){
        return NULL;
    }

    return vec->array;
}

void vector_free(vector_t* vec){
    free(vec->array);
    vec->array = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

void vector_free_destructor(vector_t* vec, void (*destructor)(void*)) {
    while (!vector_empty(vec)) {
        void* element = vector_pop_back(vec);
        destructor(element);
    }

    vector_free(vec);
}

void* vector_get(const vector_t* vec, int index){
    if (index >= vector_size(vec)){
        return NULL;
    }

    return (char*)vec->array + index * vec->element_size;
}

void vector_insert(vector_t* vec, void* element, int index){
    //if the vector is full then reserve double its capacity
    if (vector_size(vec) + 1 > vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    }

    //shift everything over one
    for (int i = vector_size(vec) + 1; i >= index; i--){
        memcpy((char*)vec->array + i * vec->element_size, (char*)vec->array + (i - 1) * vec->element_size, vec->element_size); 
    }
    //set the element at index to the given element
    memcpy((char*)vec->array + index * vec->element_size, element, vec->element_size);
    vec->size++;
}

void* vector_pop_back(vector_t* vec){
    if (vector_size(vec) == 0){
        return NULL;
    } 

    //if the size of the vector drops to 25% of its capacity then halve its capacity
    if ((vector_size(vec) - 1) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }

    vec->size--;
    void* element = (char*)vec->array + vec->size * vec->element_size;

    return element;
}

void vector_push_back(vector_t* vec, void* element) {
    // if the array is full then double its capacity  
    if (vector_size(vec) == vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    } 
    
    memcpy((char*)vec->array + vec->size * vec->element_size, element, vec->element_size);
    vec->size++;
}

void vector_reserve(vector_t* vec, int new_capacity) {
    void* ptr = realloc(vec->array, vec->element_size * new_capacity);
    vec->array = ptr;

    if (vec->array == NULL){
        return;
    }
    
    vec->capacity = new_capacity;
}

void vector_resize(vector_t* vec, int new_size){
    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * new_size);
    } 
    
    vec->size = new_size;
}

void vector_shrink_to_fit(vector_t* vec){
    vec->capacity = vec->size;
}

int vector_size(const vector_t* vec) {
    return vec->size;
}

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

void vector_upsize(vector_t* vec, int new_size, void* element){
    if (new_size < vector_size(vec)){
        return;
    } 
    
    int old_size = vec->size; 
    vec->size = new_size;

    if (vector_size(vec) > vector_capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }

    for (int i = old_size; i < new_size; i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}


