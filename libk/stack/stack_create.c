#include <stack.h>

stack_t stack_create(size_t element_size) {
    stack_t stack;
    stack.stack = kmalloc(element_size * 10); // starts with a capacity of 10 values 
    stack.size = 0;
    stack.capacity = 10;
    stack.top = -1; // must start it at -1 to make it a 0 based index
    stack.element_size = element_size;
        
    return stack;
}
