#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

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
#define SYS_GETDENTS    141
#define SYS_GETCWD      183

typedef struct {
    char name[128];
    uint32_t inode;
} dirent_t;

typedef uint16_t mode_t;

#define S_IFMT  0170000 // file type mask
#define S_IRWXU 0000700 // user has read, write, and execute
#define S_IRUSR 0000400 // user has read
#define S_IWUSR 0000200 // user has write
#define S_IXUSR 0000100 // user has execute
#define S_IRWXG 0000070 // group has read, write, and execute
#define S_IRGRP 0000040 // group has read
#define S_IWGRP 0000020 // group has write
#define S_IXGRP 0000010 // group has execute
#define S_IRWXO 0000007 // others have read, write, and execute
#define S_IROTH 0000004 // others have read
#define S_IWOTH 0000002 // others have write
#define S_IXOTH 0000001 // others have execute

// NOT IMPLEMENTED YET
#define S_ISUID 04000 // set-user-ID
#define S_ISGID 02000 // set-group-id
#define S_ISVTX 01000 // sticky bit

#define O_RDONLY    00000000
#define O_WRONLY    00000001
#define O_RDWR      00000002
#define O_ACCMODE   00000003

#define O_CREAT     00000100
#define O_EXCL      00000200
#define O_NOCTTY    00000400
#define O_TRUNC     00001000

#define O_APPEND    00002000
#define O_NONBLOCK  00004000
#define O_DYSNC     00010000
#define O_ASYNC     00020000
#define O_DIRECT    00040000
#define O_LARGEFILE 00100000
#define O_DIRECTORY 00200000
#define O_NOFOLLOW  00400000
#define O_NOATIME   01000000
#define O_CLOEXEC   02000000
#define O_SYNC      04000000

// not yet implemented
#define O_NDELAY
#define O_PATH
#define O_TMPFILE

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


#endif
