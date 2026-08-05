#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include <stdint.h>
#include <stdio.h>

#include <arch/exec/proc.h>
#include <arch/interrupts/idt.h>
#include <arch/interrupts/pic.h>

#include "exec/process.h"

extern volatile uint32_t ticks;

void timer_callback(arch_trapframe_t* tf);
uint32_t timer_get_ticks();

#endif // !INCLUDE_TIMER_H_
