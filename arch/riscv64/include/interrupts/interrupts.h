#ifndef ARCH_RISCV64_INCLUDE_INTERRUPTS_INTERRUPTS_H_
#define ARCH_RISCV64_INCLUDE_INTERRUPTS_INTERRUPTS_H_

#include "trapframe.h"

#define SCAUSE_CODE_MASK       (~(1ULL << 63))

#define SCAUSE_SUPERVISOR_SOFTWARE_IRQ  1
#define SCAUSE_SUPERVISOR_TIMER_IRQ     5
#define SCAUSE_SUPERVISOR_EXTERNAL_IRQ  9

void riscv_interrupt_dispatcher(arch_trapframe_t* tf);

#endif
