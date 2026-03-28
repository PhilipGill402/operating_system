#ifndef INCLUDE_TTY_H_
#define INCLUDE_TTY_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory/heap.h"
#include "memory/physical_allocator.h"
#include "vga.h"
#include "timer.h"

#define INPUT_BUFFER_SIZE 128

typedef struct {
    char data[INPUT_BUFFER_SIZE];
    uint32_t length;
    volatile uint8_t ready;
} input_buffer_t;

extern uint32_t heap_curr; 
extern uint32_t heap_end;
extern pmm_t pmm;

void input_buffer_reset();
void input_buffer_push(char c);
void input_buffer_pop();
void input_buffer_submit();
void read_command();
void tty();

#endif // !INCLUDE_TTY_H_
