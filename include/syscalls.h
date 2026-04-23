#ifndef INCLUDE_SYSCALLS_H_
#define INCLUDE_SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>

#include "exec/process.h"
#include "exec/scheduler.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "interrupts/registers.h"
#include "tty/tty.h"
#include "fs/initrd.h"
#include "fs/fs.h"

#define PAGE_SIZE 4096

#define SYS_READ    0
#define SYS_WRITE   1
#define SYS_CHDIR   12
#define SYS_GETPID  20
#define SYS_BRK     45
#define SYS_FORK    57
#define SYS_EXECVE  59
#define SYS_EXIT    60
#define SYS_GETCWD  183

void syscall_handler(regs_t* reg);

#endif // !INCLUDE_SYSCALLS_H_


