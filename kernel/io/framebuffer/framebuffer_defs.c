#include "io/framebuffer/framebuffer_defs.h"

framebuffer_t framebuffer;
uint8_t framebuffer_initialized = 0;
volatile uint8_t cursor_on;

fb_shared_buffer_t fb_shared_buffer;

uint32_t framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;
    
    framebuffer.backbuffer[y * framebuffer.width + x] = color;

    return 1;
}

uint32_t framebuffer_set_pixel_raw(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;
    
    uint32_t* pixel = (uint32_t*)(framebuffer.addr + y * framebuffer.pitch + x * sizeof(uint32_t));
    *pixel = color; 
    
    return 1;
}
