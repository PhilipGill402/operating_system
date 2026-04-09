#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdlib.h>
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


typedef struct queue_t{
    int size;
    int capacity;
    int head;
    int rear;
    size_t element_size;
    void* array;
} queue_t;

queue_t queue_create(size_t element_size);
void queue_free(queue_t* queue);
bool queue_resize(queue_t* queue, int new_capacity);
int enqueue(queue_t* queue, void* val);
void* dequeue(queue_t* queue);
void* queue_first(const queue_t* queue);
int queue_empty(const queue_t* queue);
int queue_size(const queue_t* queue);

#endif
