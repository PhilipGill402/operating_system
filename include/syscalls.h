#ifndef INCLUDE_SYSCALLS_H_
#define INCLUDE_SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>

#include <errno.h>
#include <sys/io.h>
#include <sys/signals.h>
#include <sys/mman.h>
#include <sys/fb.h>
#include <string.h>

#include "io/framebuffer.h"
#include "exec/process.h"
#include "exec/scheduler.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "interrupts/registers.h"
#include "timer.h"
#include "fs/fs.h"

#define PAGE_SIZE 4096

#define SYS_READ        0
#define SYS_WRITE       1
#define SYS_OPEN        5
#define SYS_CLOSE       6
#define SYS_WAITPID     7
#define SYS_CHDIR       12
#define SYS_LSEEK       19
#define SYS_GETPID      20
#define SYS_KILL        37
#define SYS_MKDIR       39
#define SYS_DUP         41
#define SYS_BRK         45
#define SYS_FORK        57
#define SYS_EXECVE      59
#define SYS_EXIT        60
#define SYS_DUP2        63
#define SYS_MMAP        90
#define SYS_MUNMAP      91
#define SYS_GETDENTS    141
#define SYS_POLL        168
#define SYS_GETCWD      183
#define SYS_FB_INFO     200
#define SYS_FB_FLUSH    201
#define SYS_YIELD       202

void syscall_handler(regs_t* reg);

#endif // !INCLUDE_SYSCALLS_H_


