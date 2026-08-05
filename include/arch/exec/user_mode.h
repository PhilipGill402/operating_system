#ifndef INCLUDE_ARCH_EXEC_USER_MODE_H_
#define INCLUDE_ARCH_EXEC_USER_MODE_H_

#include <arch/exec/proc.h>

__attribute__((noreturn)) void arch_return_to_user(const arch_trapframe_t* tf);

#endif
