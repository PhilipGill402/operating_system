#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef __is_libk
#include "memory/heap.h"
#define malloc(size)        kmalloc(size)
#define free(ptr)           kfree(ptr)
#define realloc(ptr, size)  krealloc(ptr, size)
#else
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
#endif

typedef struct vector_t{
    void* array; 
    int size;
    int capacity;
    size_t element_size;
} vector_t; 

typedef struct vector_iterator_t{
    void* current;
    void* end;
    size_t element_size;
} vector_iterator_t;

//initializing and destroying
vector_t vector_create(size_t element_size);
void vector_free(vector_t* vec);

//managing size
void vector_reserve(vector_t* vec, int new_capacity);
void vector_resize(vector_t* vec, int new_capacity);
int vector_size(const vector_t* vec);
int vector_capacity(const vector_t* vec);
void vector_downsize(vector_t* vec, int new_size);
void vector_upsize(vector_t* vec, int new_size, void* element);
void vector_shrink_to_fit(vector_t* vec);

//adding / deleting values
void vector_push_back(vector_t* vec, void* element);
void* vector_pop_back(vector_t* vec);

//accessing data
void* vector_get(const vector_t* vec, int index);
void* vector_front(const vector_t* vec);
void* vector_back(const vector_t* vec);
void vector_insert(vector_t* vec, void* element, int index);
void vector_erase(vector_t* vec, int index);
void vector_swap(vector_t* vec, vector_t* other_vec);
void vector_clear(vector_t* vec);
void vector_assign(vector_t* vec, void* element, int num_copies);
void* vector_data(const vector_t* vec);

//iterators
vector_iterator_t* vector_iterator(vector_t* vec);
vector_iterator_t* vector_create_iterator(const vector_t* vec);
int vector_has_next(const vector_iterator_t* itr);
void* vector_next(vector_iterator_t* itr);

//miscellanenous
bool vector_empty(const vector_t* vec);
void vector_print(const vector_t* vec, void (*print_element)(void*));

#endif // !UTILS_VECTOR_H_
