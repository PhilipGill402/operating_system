#ifndef LIBC_SYS_FB_H_
#define LIBC_SYS_FB_H_

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t size;
} fb_info_t;

#endif // !LIBC_SYS_FB_H_
