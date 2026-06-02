#ifndef INCLUDE_SYSCALL_DEFS_H_
#define INCLUDE_SYSCALL_DEFS_H_

#include <stdint.h>

#define EPERM           -1
#define ENOENT          -2
#define ESRCH           -3
#define EINTR           -4
#define EIO             -5
#define ENXIO           -6
#define E2BIG           -7
#define ENOEXEC         -8
#define EBADF           -9
#define ECHILD          -10
#define EAGAIN          -11
#define EWOULDBLOCK     -11
#define ENOMEM          -12
#define EACCES          -13
#define EFAULT          -14
#define ENOTBLK         -15
#define EBUSY           -16
#define EEXIST          -17
#define EXDEV           -18
#define ENODEV          -19
#define ENOTDIR         -20
#define EISDIR          -21
#define EINVAL          -22
#define ENFILE          -23
#define EMFILE          -24
#define ENOTTY          -25
#define ETXTBSY         -26
#define EFBIG           -27
#define ENOSPC          -28
#define ESPIPE          -29
#define EROFS           -30
#define EMLINK          -31
#define EPIPE           -32
#define EDOM            -33
#define ERANGE          -34
#define EDEADLK         -35
#define EDEADLOCK       -35
#define ENAMETOOLONG    -36
#define ENOLCK          -37
#define ENOSYS          -38
#define ENOTEMPTY       -39
#define ELOOP           -40
#define ENOMSG          -42
#define EIDRM           -43
#define ECHRNG          -44
#define EL2NSYNC        -45
#define EL3HLT          -46
#define EL3RST          -47
#define ELNRNG          -48
#define EUNATCH         -49
#define ENOCSI          -50
#define EL2HLT          -51
#define EBADE           -52
#define EBADR           -53
#define EXFULL          -54
#define ENOANO          -55
#define EBADRQC         -56
#define EBADSLT         -57
#define EBFONT          -59
#define ENOSTR          -60
#define ENODATA         -61
#define ETIME           -62
#define ENOSR           -63
#define ENONET          -64
#define ENOPKG          -65
#define EREMOTE         -66
#define ENOLINK         -67
#define EADV            -68
#define ESRMNT          -69
#define ECOMM           -70
#define EPROTO          -71
#define EMULTIHOP       -72
#define EDOTDOT         -73
#define EBADMSG         -74
#define EOVERFLOW       -75
#define ENOTUNIQ        -76
#define EBADFD          -77
#define EREMCHG         -78
#define ELIBACC         -79
#define ELIBBAD         -80
#define ELIBSCN         -81
#define ELIBMAX         -82
#define ELIBEXEC        -83
#define EILSEQ          -84
#define ERESTART        -85
#define ESTRPIPE        -86
#define EUSERS          -87
#define ENOTSOCK        -88
#define EDESTADDRREQ    -89
#define EMSGSIZE        -90
#define EPROTOTYPE      -91
#define ENOPROTOOPT     -92
#define EPROTONOSUPPORT -93
#define ESOCKTNOSUPPORT -94
#define EOPNOTSUPP      -95
#define ENOTSUP         -95
#define EPFNOSUPPORT    -96
#define EAFNOSUPPORT    -97
#define EADDRINUSE      -98
#define EADDRNOTAVAIL   -99
#define ENETDOWN        -100
#define ENETUNREACH     -101
#define ENETRESET       -102
#define ECONNABORTED    -103
#define ECONNRESET      -104
#define ENOBUFS         -105
#define EISCONN         -106
#define ENOTCONN        -107
#define ESHUTDOWN       -108
#define ETOOMANYREFS    -109
#define ETIMEDOUT       -110
#define ECONNREFUSED    -111
#define EHOSTDOWN       -112
#define EHOSTUNREACH    -113
#define EALREADY        -114
#define EINPROGRESS     -115
#define ESTALE          -116
#define EUCLEAN         -117
#define ENOTNAM         -118
#define ENAVAIL         -119
#define EISNAM          -120
#define EREMOTEIO       -121
#define EDQUOT          -122
#define ENOMEDIUM       -123
#define EMEDIUMTYPE     -124
#define ECANCELED       -125
#define ENOKEY          -126
#define EKEYEXPIRED     -127
#define EKEYREVOKED     -128
#define EKEYREJECTED    -129
#define EOWNERDEAD      -130
#define ENOTRECOVERABLE -131
#define ERFKILL         -132
#define EHWPOISON       -133

typedef struct {
    char name[128];
    uint32_t inode;
} sys_dirent_t;

typedef uint16_t sys_mode_t;

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

#define SIGTERM 1
#define SIGKILL 2
#define SIGSTOP 4
#define SIGCONT 8



#endif // !INCLUDE_SYSCALL_DEFS_H_
