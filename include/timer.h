#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include <stdint.h>
#include <stdio.h>
#include "interrupts/idt.h"
#include "interrupts/registers.h"
#include "exec/process.h"
#include "interrupts/pic.h"

extern volatile uint32_t ticks;

void timer_callback(regs_t* r);
uint32_t timer_get_ticks();

#endif // !INCLUDE_TIMER_H_
