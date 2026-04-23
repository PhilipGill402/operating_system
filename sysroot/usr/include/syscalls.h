#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>

#define SYS_READ    0
#define SYS_WRITE   1
#define SYS_CHDIR   12
#define SYS_GETPID  20
#define SYS_BRK     45
#define SYS_FORK    57
#define SYS_EXECVE  59
#define SYS_EXIT    60
#define SYS_GETCWD  183

/* HELPERS */
uint32_t __sys0(uint32_t sys_num);
uint32_t __sys1(uint32_t sys_num, uint32_t arg1);
uint32_t __sys2(uint32_t sys_num, uint32_t arg1, uint32_t arg2);
uint32_t __sys3(uint32_t sys_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

uint32_t read(uint32_t fd, char* buffer, size_t count);
uint32_t write(uint32_t fd, const char* buffer, size_t count);
void exit(int32_t error_code);
uint32_t fork();
uint32_t execve(const char* path, const char* argv[]);
uint32_t getcwd(char* buffer, size_t size);
uint32_t chdir(const char* path);
uint32_t getpid();
void* brk();

#endif
