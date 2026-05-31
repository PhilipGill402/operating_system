#ifndef ERRNO_H_
#define ERRNO_H_

#define EPERM           1
#define ENOENT          2
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7
#define ENOEXEC         8
#define EBADF           9
#define ECHILD          10
#define EAGAIN          11
#define EWOULDBLOCK     11
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define ENOTBLK         15
#define EBUSY           16
#define EEXIST          17
#define EXDEV           18
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOTTY          25
#define ETXTBSY         26
#define EFBIG           27
#define ENOSPC          28
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33
#define ERANGE          34
#define EDEADLK         35
#define EDEADLOCK       35
#define ENAMETOOLONG    36
#define ENOLCK          37
#define ENOSYS          38
#define ENOTEMPTY       39
#define ELOOP           40
#define ENOMSG          42
#define EIDRM           43
#define ECHRNG          44
#define EL2NSYNC        45
#define EL3HLT          46
#define EL3RST          47
#define ELNRNG          48
#define EUNATCH         49
#define ENOCSI          50
#define EL2HLT          51
#define EBADE           52
#define EBADR           53
#define EXFULL          54
#define ENOANO          55
#define EBADRQC         56
#define EBADSLT         57
#define EBFONT          59
#define ENOSTR          60
#define ENODATA         61
#define ETIME           62
#define ENOSR           63
#define ENONET          64
#define ENOPKG          65
#define EREMOTE         66
#define ENOLINK         67
#define EADV            68
#define ESRMNT          69
#define ECOMM           70
#define EPROTO          71
#define EMULTIHOP       72
#define EDOTDOT         73
#define EBADMSG         74
#define EOVERFLOW       75
#define ENOTUNIQ        76
#define EBADFD          77
#define EREMCHG         78
#define ELIBACC         79
#define ELIBBAD         80
#define ELIBSCN         81
#define ELIBMAX         82
#define ELIBEXEC        83
#define EILSEQ          84
#define ERESTART        85
#define ESTRPIPE        86
#define EUSERS          87
#define ENOTSOCK        88
#define EDESTADDRREQ    89
#define EMSGSIZE        90
#define EPROTOTYPE      91
#define ENOPROTOOPT     92
#define EPROTONOSUPPORT 93
#define ESOCKTNOSUPPORT 94
#define EOPNOTSUPP      95
#define ENOTSUP         95
#define EPFNOSUPPORT    96
#define EAFNOSUPPORT    97
#define EADDRINUSE      98
#define EADDRNOTAVAIL   99
#define ENETDOWN        100
#define ENETUNREACH     101
#define ENETRESET       102
#define ECONNABORTED    103
#define ECONNRESET      104
#define ENOBUFS         105
#define EISCONN         106
#define ENOTCONN        107
#define ESHUTDOWN       108
#define ETOOMANYREFS    109
#define ETIMEDOUT       110
#define ECONNREFUSED    111
#define EHOSTDOWN       112
#define EHOSTUNREACH    113
#define EALREADY        114
#define EINPROGRESS     115
#define ESTALE          116
#define EUCLEAN         117
#define ENOTNAM         118
#define ENAVAIL         119
#define EISNAM          120
#define EREMOTEIO       121
#define EDQUOT          122
#define ENOMEDIUM       123
#define EMEDIUMTYPE     124
#define ECANCELED       125
#define ENOKEY          126
#define EKEYEXPIRED     127
#define EKEYREVOKED     128
#define EKEYREJECTED    129
#define EOWNERDEAD      130
#define ENOTRECOVERABLE 131
#define ERFKILL         132
#define EHWPOISON       133

static const char* errno_strings[] = {
    [0]   = "Success",
    [1]   = "Operation not permitted",
    [2]   = "No such file or directory",
    [3]   = "No such process",
    [4]   = "Interrupted system call",
    [5]   = "Input/output error",
    [6]   = "No such device or address",
    [7]   = "Argument list too long",
    [8]   = "Exec format error",
    [9]   = "Bad file descriptor",
    [10]  = "No child processes",
    [11]  = "Resource temporarily unavailable",
    [12]  = "Cannot allocate memory",
    [13]  = "Permission denied",
    [14]  = "Bad address",
    [15]  = "Block device required",
    [16]  = "Device or resource busy",
    [17]  = "File exists",
    [18]  = "Invalid cross-device link",
    [19]  = "No such device",
    [20]  = "Not a directory",
    [21]  = "Is a directory",
    [22]  = "Invalid argument",
    [23]  = "Too many open files in system",
    [24]  = "Too many open files",
    [25]  = "Inappropriate ioctl for device",
    [26]  = "Text file busy",
    [27]  = "File too large",
    [28]  = "No space left on device",
    [29]  = "Illegal seek",
    [30]  = "Read-only file system",
    [31]  = "Too many links",
    [32]  = "Broken pipe",
    [33]  = "Numerical argument out of domain",
    [34]  = "Numerical result out of range",
    [35]  = "Resource deadlock avoided",
    [36]  = "File name too long",
    [37]  = "No locks available",
    [38]  = "Function not implemented",
    [39]  = "Directory not empty",
    [40]  = "Too many levels of symbolic links",
    [41]  = "Unknown error",
    [42]  = "No message of desired type",
    [43]  = "Identifier removed",
    [44]  = "Channel number out of range",
    [45]  = "Level 2 not synchronized",
    [46]  = "Level 3 halted",
    [47]  = "Level 3 reset",
    [48]  = "Link number out of range",
    [49]  = "Protocol driver not attached",
    [50]  = "No CSI structure available",
    [51]  = "Level 2 halted",
    [52]  = "Invalid exchange",
    [53]  = "Invalid request descriptor",
    [54]  = "Exchange full",
    [55]  = "No anode",
    [56]  = "Invalid request code",
    [57]  = "Invalid slot",
    [58]  = "Unknown error",
    [59]  = "Bad font file format",
    [60]  = "Device not a stream",
    [61]  = "No data available",
    [62]  = "Timer expired",
    [63]  = "Out of streams resources",
    [64]  = "Machine is not on the network",
    [65]  = "Package not installed",
    [66]  = "Object is remote",
    [67]  = "Link has been severed",
    [68]  = "Advertise error",
    [69]  = "Srmount error",
    [70]  = "Communication error on send",
    [71]  = "Protocol error",
    [72]  = "Multihop attempted",
    [73]  = "RFS specific error",
    [74]  = "Bad message",
    [75]  = "Value too large for defined data type",
    [76]  = "Name not unique on network",
    [77]  = "File descriptor in bad state",
    [78]  = "Remote address changed",
    [79]  = "Can not access a needed shared library",
    [80]  = "Accessing a corrupted shared library",
    [81]  = ".lib section in a.out corrupted",
    [82]  = "Attempting to link in too many shared libraries",
    [83]  = "Cannot exec a shared library directly",
    [84]  = "Invalid or incomplete multibyte or wide character",
    [85]  = "Interrupted system call should be restarted",
    [86]  = "Streams pipe error",
    [87]  = "Too many users",
    [88]  = "Socket operation on non-socket",
    [89]  = "Destination address required",
    [90]  = "Message too long",
    [91]  = "Protocol wrong type for socket",
    [92]  = "Protocol not available",
    [93]  = "Protocol not supported",
    [94]  = "Socket type not supported",
    [95]  = "Operation not supported",
    [96]  = "Protocol family not supported",
    [97]  = "Address family not supported by protocol",
    [98]  = "Address already in use",
    [99]  = "Cannot assign requested address",
    [100] = "Network is down",
    [101] = "Network is unreachable",
    [102] = "Network dropped connection on reset",
    [103] = "Software caused connection abort",
    [104] = "Connection reset by peer",
    [105] = "No buffer space available",
    [106] = "Transport endpoint is already connected",
    [107] = "Transport endpoint is not connected",
    [108] = "Cannot send after transport endpoint shutdown",
    [109] = "Too many references: cannot splice",
    [110] = "Connection timed out",
    [111] = "Connection refused",
    [112] = "Host is down",
    [113] = "No route to host",
    [114] = "Operation already in progress",
    [115] = "Operation now in progress",
    [116] = "Stale file handle",
    [117] = "Structure needs cleaning",
    [118] = "Not a XENIX named type file",
    [119] = "No XENIX semaphores available",
    [120] = "Is a named type file",
    [121] = "Remote I/O error",
    [122] = "Disk quota exceeded",
    [123] = "No medium found",
    [124] = "Wrong medium type",
    [125] = "Operation canceled",
    [126] = "Required key not available",
    [127] = "Key has expired",
    [128] = "Key has been revoked",
    [129] = "Key was rejected by service",
    [130] = "Owner died",
    [131] = "State not recoverable",
    [132] = "Operation not possible due to RF-kill",
    [133] = "Memory page has hardware error",
};

extern int errno;

const char* strerror(int errnum);

#endif /* ERRNO_H_ */
