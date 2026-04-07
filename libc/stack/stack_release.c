#include <stack.h>

void stack_release(stack_t* stack){
    free(stack->stack);
    stack->stack = NULL;
    stack->size = 0;
}
