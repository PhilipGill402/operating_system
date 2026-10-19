#ifndef LIBC_INCLUDE_SYS_POLL_H_
#define LIBC_INCLUDE_SYS_POLL_H_

#include <stdint.h>

#define POLLIN   0x01
#define POLLOUT  0x02
#define POLLERR  0x04
#define POLLHUP  0x08

typedef struct {
    uint32_t fd;
    uint8_t events;
    uint8_t revents;
} pollfd_t;

#endif // !LIBC_INCLUDE_SYS_POLL_H_
