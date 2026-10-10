#ifndef INCLUDE_INTERRUPTS_PORT_H_
#define INCLUDE_INTERRUPTS_PORT_H_

#include <stdint.h>

void arch_write_byte(uintptr_t port, uint8_t value);
uint8_t arch_read_byte(uintptr_t port);
uint32_t arch_read_u32(uintptr_t addr);
void arch_write_u32(uintptr_t addr, uint32_t value);

void arch_io_wait(void);

#endif // !INCLUDE_INTERRUPTS_PORT_H_

