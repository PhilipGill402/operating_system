#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stddef.h>
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


typedef struct stack_s {
    int size;
    int capacity;
    int top;
    size_t element_size;
    void* stack; 
} stack_t;

stack_t stack_create(size_t element_size);
int stack_push(stack_t* stack, void* val);
void* stack_pop(stack_t* stack);
void* stack_top(const stack_t* stack);
int stack_size(const stack_t* stack);
int stack_empty(const stack_t* stack);
void stack_release(stack_t* stack);

#endif // !STACK_H_

