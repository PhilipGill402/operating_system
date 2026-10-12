#ifndef ARCH_RISCV64_INCLUDE_EXEC_CONTEXT_H_
#define ARCH_RISCV64_INCLUDE_EXEC_CONTEXT_H_

#include <stdint.h>
#include "trapframe.h"

typedef struct arch_kernel_context {
    uint64_t ra;
    uint64_t sp;
    uint64_t s0, s1, s2, s3, s4, s5;
    uint64_t s6, s7, s8, s9, s10, s11;
} arch_kernel_context_t;

typedef struct arch_context {
    arch_trapframe_t* tf;
    arch_kernel_context_t kernel_ctx;
    uint64_t satp;
    uint64_t kernel_sp;
} arch_context_t;

#endif
