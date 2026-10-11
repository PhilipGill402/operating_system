#ifndef INCLUDE_IO_INPUT_BUFFER_H_
#define INCLUDE_IO_INPUT_BUFFER_H_

#include <stdint.h>

#define INPUT_BUFFER_SIZE 255

typedef struct input_buffer {
    uint8_t size;
    uint8_t write_idx;
    uint8_t read_idx;
    char* buffer;
} input_buffer_t;

void input_buffer_alloc(void);
void input_buffer_write_char(char c);
char input_buffer_read_char(void);

extern input_buffer_t input_buffer;

#endif
