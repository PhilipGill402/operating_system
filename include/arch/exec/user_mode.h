#ifndef INCLUDE_ARCH_EXEC_USER_MODE_H_
#define INCLUDE_ARCH_EXEC_USER_MODE_H_

#include <arch/registers.h>

__attribute__((noreturn)) void enter_user_mode_from_trapframe(const regs_t* tf);

#endif
