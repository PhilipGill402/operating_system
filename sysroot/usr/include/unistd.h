#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/io.h>
#include <sys/fb.h>

#define SYS_READ        0
#define SYS_WRITE       1
#define SYS_OPEN        5
#define SYS_CLOSE       6
#define SYS_WAITPID     7
#define SYS_CHDIR       12
#define SYS_LSEEK       19
#define SYS_GETPID      20
#define SYS_KILL        39
#define SYS_BRK         45
#define SYS_FORK        57
#define SYS_EXECVE      59
#define SYS_EXIT        60
#define SYS_MMAP        90
#define SYS_MUNMAP      91
#define SYS_GETDENTS    141
#define SYS_GETCWD      183
#define SYS_FB_INFO     200

/* HELPERS */
int32_t __sys0(uint32_t sys_num);
int32_t __sys1(uint32_t sys_num, uint32_t arg1);
int32_t __sys2(uint32_t sys_num, uint32_t arg1, uint32_t arg2);
int32_t __sys3(uint32_t sys_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

/* SYS Calls */
int32_t read(uint32_t fd, char* buffer, size_t count);
int32_t write(uint32_t fd, const char* buffer, size_t count);
void exit(int32_t error_code);
int32_t fork();
int32_t execve(const char* path, const char* argv[]);
int32_t getcwd(char* buffer, size_t size);
int32_t chdir(const char* path);
int32_t getpid();
void* brk(void* new_addr);
void* sbrk(size_t increment);
int32_t open(const char* path, uint32_t flags, mode_t mode);
int32_t getdents(uint32_t fd, dirent_t* dents, uint32_t count);
int32_t waitpid(uint32_t pid, int* status, uint32_t options);
int32_t close(uint32_t fd);
int32_t lseek(uint32_t fd, uint32_t offset);
int32_t kill(uint32_t pid, int32_t sig);
void* mmap(void* addr, uint32_t length, int32_t prot, int32_t flags, int32_t fd, uint32_t offset);
int32_t munmap(void* addr, uint32_t length);
int32_t fb_info(fb_info_t* info);

#endif
