#ifndef INCLUDE_INTERRUPTS_KEYBOARD_H_
#define INCLUDE_INTERRUPTS_KEYBOARD_H_

#include <stdint.h>
#include <stdio.h>
#include "interrupts/idt.h"
#include "interrupts/port.h"
#include "tty/tty.h"

static volatile uint8_t last_scancode = 0;

void keyboard_callback(regs_t* r);

#endif // !INCLUDE_INTERRUPTS_KEYBOARD_H_
