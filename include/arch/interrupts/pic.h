#ifndef INCLUDE_INTERRUPT_PIC_H_
#define INCLUDE_INTERRUPT_PIC_H_

#include <stdint.h>

#include <arch/interrupts/port.h>

#define IRQ_TIMER    0
#define IRQ_KEYBOARD 1

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_ICW4    0x01
#define ICW1_INIT    0x10

#define ICW4_8086    0x01

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_set_mask(uint8_t irq_line);
void pic_clear_mask(uint8_t irq_line);
void pic_send_eoi(uint8_t irq_line);
#endif // !INCLUDE_INTERRUPT_PIC_H_
