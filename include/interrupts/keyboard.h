#ifndef INCLUDE_INTERRUPTS_KEYBOARD_H_
#define INCLUDE_INTERRUPTS_KEYBOARD_H_

#include <stdint.h>
#include <stdio.h>
#include "interrupts/idt.h"
#include "interrupts/port.h"


#define INPUT_BUFFER_SIZE 128

static volatile uint8_t last_scancode = 0;

typedef struct {
    char data[INPUT_BUFFER_SIZE];
    volatile uint32_t length;
    volatile uint8_t ready;
} input_buffer_t;

void keyboard_callback(regs_t* r);

void input_buffer_reset();
void input_buffer_push(char c);
char input_buffer_pop();
void input_buffer_submit();
uint8_t input_buffer_empty();
uint8_t input_buffer_ready();
char* input_buffer_data();
uint32_t input_buffer_length();

extern input_buffer_t keyboard_buffer;

#endif // !INCLUDE_INTERRUPTS_KEYBOARD_H_
