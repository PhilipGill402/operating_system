#ifndef INCLUDE_SYSCALLS_H_
#define INCLUDE_SYSCALLS_H_

#include "exec/elf.h"
#include "interrupts/registers.h"

#define SYS_EXIT 1

void syscall_handler(regs_t* reg);

#endif // !INCLUDE_SYSCALLS_H_


