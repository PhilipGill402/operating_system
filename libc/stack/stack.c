#include <stack.h>

stack_t stack_create(size_t element_size) {
    stack_t stack;
    stack.stack = malloc(element_size * 10); // starts with a capacity of 10 values 
    stack.size = 0;
    stack.capacity = 10;
    stack.top = -1; // must start it at -1 to make it a 0 based index
    stack.element_size = element_size;
        
    return stack;
}

int stack_empty(const stack_t* stack){
    return stack->size == 0;
}

int stack_push(stack_t* stack, void* val){
    //checks if stack is full and if so, it doubles its capacity
    if (stack->size == stack->capacity){
        stack->stack = realloc(stack->stack, sizeof(void*) * (stack->size * 2));
        
        if (!stack->stack){
            return 0;
        }
        
        stack->capacity *= 2;
    }
    
    memcpy((char*)stack->stack + stack->size * stack->element_size, val, stack->element_size);
    stack->top++;
    stack->size++;

    return 1;
}

void stack_release(stack_t* stack){
    free(stack->stack);
    stack->stack = NULL;
    stack->size = 0;
}

int stack_size(const stack_t* stack){
    return stack->size;
}

void* stack_top(const stack_t* stack){
    if (stack_empty(stack)){
        return NULL;
    }

    void* val = (char*)stack->stack + stack->top * stack->element_size;
    return val;
}
