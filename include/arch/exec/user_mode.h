#ifndef INCLUDE_ARCH_EXEC_USER_MODE_H_
#define INCLUDE_ARCH_EXEC_USER_MODE_H_

#include <arch/exec/proc.h>
#include <stdint.h>

__attribute__((noreturn)) void arch_return_to_user(const arch_trapframe_t* tf, uintptr_t kernel_stack_top);
void arch_context_switch(arch_context_t* old_ctx, arch_context_t* new_ctx);

#endif
