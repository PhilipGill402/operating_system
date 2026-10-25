#ifndef ARCH_RISCV64_INCLUDE_DRIVERS_UART_H_
#define ARCH_RISCV64_INCLUDE_DRIVERS_UART_H_

#include "trapframe.h"
#include "kernel.h"

void uart_init(void);
void uart_handler(arch_trapframe_t* tf);

#endif
