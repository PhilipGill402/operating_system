#ifndef LIBK_SYS_FB_H_
#define LIBK_SYS_FB_H_

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t size;
} sys_fb_info_t;

#endif // !LIBK_SYS_FB_H_
