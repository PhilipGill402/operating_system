#include <stack.h>

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
