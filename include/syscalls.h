#ifndef INCLUDE_SYSCALLS_H_
#define INCLUDE_SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>

#include "exec/process.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "interrupts/registers.h"

#define PAGE_SIZE 4096
#define SYS_EXIT 1

void syscall_handler(regs_t* reg);

#endif // !INCLUDE_SYSCALLS_H_


