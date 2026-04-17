#include <stack.h>

void stack_release(stack_t* stack){
    kfree(stack->stack);
    stack->stack = NULL;
    stack->size = 0;
}
