#ifndef INCLUDE_INTERRUPTS_IRQ_H_
#define INCLUDE_INTERRUPTS_IRQ_H_

#include <stdint.h>
#include <stdio.h>
#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/keyboard.h"
#include "timer.h"

typedef void(*irq_handler_t)(regs_t* r);

static irq_handler_t irq_routines[16] = {0};

void irq_install_handler(uint8_t irq, irq_handler_t handler);
void irq_uninstall_handler(uint8_t irq);
void irq_handler(regs_t* r);
void irq_init_handlers();

#endif // !INCLUDE_INTERRUPTS_IRQ_H_

