#ifndef LIBC_SYS_IO_H_
#define LIBC_SYS_IO_H_

typedef struct {
    char name[128];
    uint32_t inode;
} dirent_t;

typedef struct {
    uint32_t type;
    uint32_t keycode;
    char ch;
    uint8_t pressed;
    uint8_t modifiers;
} input_event_t;

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



#endif // !LIBC_SYS_IO_H_
