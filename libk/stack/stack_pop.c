#include <stack.h>

void* stack_pop(stack_t* stack){
    if (stack_empty(stack)){
        return NULL;
    }    

    void* val = (char*)stack->stack + stack->top * stack->element_size;
    stack->top--;
    stack->size--;
    
    return val;
}
