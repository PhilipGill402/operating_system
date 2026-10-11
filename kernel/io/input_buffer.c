#include "io/input_buffer.h"
#include "memory/heap.h"
#include <stdlib.h>

input_buffer_t input_buffer = { 0 };

void input_buffer_alloc(void) {
    input_buffer.size = 0;
    input_buffer.write_idx = 0;
    input_buffer.read_idx = 0;
    input_buffer.buffer = kmalloc(INPUT_BUFFER_SIZE * sizeof(char));
    if (!input_buffer.buffer)
        panic("failed to allocate input buffer");
}

void input_buffer_write_char(char c) {
    input_buffer.buffer[input_buffer.write_idx] = c;
    input_buffer.write_idx = (input_buffer.write_idx + 1) % INPUT_BUFFER_SIZE;
    input_buffer.size++;
}

char input_buffer_read_char(void) {
    char c = input_buffer.buffer[input_buffer.read_idx];
    input_buffer.read_idx = (input_buffer.read_idx + 1) % INPUT_BUFFER_SIZE;
    input_buffer.size--;
    
    return c;
}
