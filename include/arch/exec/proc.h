#ifndef INCLUDE_ARCH_EXEC_PROC_H_
#define INCLUDE_ARCH_EXEC_PROC_H_

#include <stdint.h>

typedef struct arch_trapframe arch_trapframe_t;
typedef struct arch_context arch_context_t;

#define USER_CS_RING3 0x1B
#define USER_DS_RING3 0x23
#define MAX_SEGMENTS 8

arch_trapframe_t* arch_trapframe_init(uintptr_t user_stack_top, uintptr_t entry);
void arch_trapframe_copy(arch_trapframe_t* dst, arch_trapframe_t* src);
void arch_trapframe_reset(arch_trapframe_t* tf, uintptr_t user_stack_top, uintptr_t entry);
void arch_trapframe_destroy(arch_trapframe_t* tf);
void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret);
uint32_t arch_trapframe_get_arg(arch_trapframe_t* tf, uint32_t idx);
uint8_t arch_trapframe_from_user(arch_trapframe_t* tf);

arch_context_t* arch_context_init(uintptr_t kernel_stack_top, void (*entry)(void));
void arch_context_destroy(arch_context_t* ctx);

void arch_set_kernel_stack(uint32_t stack_top);

void arch_trapframe_debug(const arch_trapframe_t* tf);

#endif
