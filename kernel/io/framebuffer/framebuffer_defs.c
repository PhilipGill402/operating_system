#include "io/framebuffer/framebuffer_defs.h"

framebuffer_t framebuffer;

uint32_t framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;

    uint32_t* pixel = (uint32_t*)(framebuffer.addr + y * framebuffer.pitch + x * 4);
    *pixel = color;

    return 1;
}
