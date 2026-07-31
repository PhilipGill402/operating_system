#ifndef INCLUDE_INTERRUPTS_PIT_H_
#define INCLUDE_INTERRUPTS_PIT_H_

#include <stdint.h>

#include <arch/interrupts/port.h>

void pit_init(uint32_t frequency);

#endif // !INCLUDE_INTERRUPTS_PIT_H_
