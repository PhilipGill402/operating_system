#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FORK 57
#define SYS_EXECVE 59
#define SYS_EXIT 60

int32_t read(uint32_t fd, char* buffer, size_t count);
int32_t write(uint32_t fd, const char* buffer, size_t count);
void exit(int32_t error_code);
int32_t fork();
int32_t execve(const char* path, const char* argv[]);

#endif
