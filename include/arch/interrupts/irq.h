#ifndef INCLUDE_INTERRUPTS_IRQ_H_
#define INCLUDE_INTERRUPTS_IRQ_H_

#include <stdint.h>
#include <stdio.h>

#include <arch/exec/proc.h>
#include <arch/interrupts/idt.h>
#include <arch/interrupts/pic.h>

#include "interrupts/keyboard.h"
#include "timer.h"

typedef void(*irq_handler_t)(arch_trapframe_t* tf);

static irq_handler_t irq_routines[16] = {0};

void irq_install_handler(uint8_t irq, irq_handler_t handler);
void irq_uninstall_handler(uint8_t irq);
void irq_handler(arch_trapframe_t* tf);
void irq_init_handlers();

#endif // !INCLUDE_INTERRUPTS_IRQ_H_

