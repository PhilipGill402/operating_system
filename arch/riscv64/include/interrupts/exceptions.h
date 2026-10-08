#ifndef ARCH_RISCV64_INCLUDE_INTERRUPTS_EXCEPTIONS_H_
#define ARCH_RISCV64_INCLUDE_INTERRUPTS_EXCEPTIONS_H_

#include "trapframe.h"

#define SCAUSE_CODE_MASK       (~(1ULL << 63))

#define SCAUSE_INST_ADDR_MISALIGNED     0
#define SCAUSE_INST_ACCESS_FAULT        1
#define SCAUSE_ILLEGAL_INSTRUCTION      2
#define SCAUSE_BREAKPOINT               3
#define SCAUSE_LOAD_ADDR_MISALIGNED     4
#define SCAUSE_LOAD_ACCESS_FAULT        5
#define SCAUSE_STORE_ADDR_MISALIGNED    6
#define SCAUSE_STORE_ACCESS_FAULT       7
#define SCAUSE_ECALL_FROM_U             8
#define SCAUSE_ECALL_FROM_S             9
#define SCAUSE_ECALL_FROM_VS            10
#define SCAUSE_ECALL_FROM_M             11
#define SCAUSE_INST_PAGE_FAULT          12
#define SCAUSE_LOAD_PAGE_FAULT          13
#define SCAUSE_STORE_PAGE_FAULT         15

void riscv_exception_dispatcher(arch_trapframe_t* tf);

#endif
