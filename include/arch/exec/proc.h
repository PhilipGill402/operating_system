#ifndef INCLUDE_ARCH_EXEC_PROC_H_
#define INCLUDE_ARCH_EXEC_PROC_H_

#include <stdint.h>

#include <exec/trapframe.h>
#include <exec/context.h>

typedef struct arch_trapframe arch_trapframe_t;
typedef struct arch_context arch_context_t;

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

arch_trapframe_t* arch_trapframe_init(uint32_t* kernel_stack_top, uintptr_t user_stack_top, uintptr_t entry);
void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret);
void arch_context_init(arch_context_t* ctx, uintptr_t kernel_stack_top, void (*entry)(void));

#endif
