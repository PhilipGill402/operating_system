#ifndef INCLUDE_IO_FRAMEBUFFER_FRAMEBUFFER_DEFS_H_
#define INCLUDE_IO_FRAMEBUFFER_FRAMEBUFFER_DEFS_H_

#include <stddef.h>
#include <stdint.h>

#include "memory_mapping.h"

#define FB_PADDING      5

#define FB_BLACK        0x00000000
#define FB_WHITE        0x00FFFFFF

#define FB_RED          0x00FF0000
#define FB_GREEN        0x0000FF00
#define FB_BLUE         0x000000FF

#define FB_YELLOW       0x00FFFF00
#define FB_CYAN         0x0000FFFF
#define FB_MAGENTA      0x00FF00FF

#define FB_GRAY         0x00808080
#define FB_DARK_GRAY    0x00404040
#define FB_LIGHT_GRAY   0x00C0C0C0

#define FB_ORANGE       0x00FFA500
#define FB_PURPLE       0x00800080
#define FB_BROWN        0x00A52A2A
#define FB_PINK         0x00FFC0CB

typedef struct {
    uint8_t bpp;
    uint32_t* backbuffer;
    uint8_t* addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t x;
    uint32_t y;
} framebuffer_t;

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t unused;
} __attribute__((packed)) fb_color_t;

uint32_t framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color);

extern framebuffer_t framebuffer;
extern uint8_t framebuffer_initialized;
extern volatile uint8_t cursor_on;

#endif // !INCLUDE_IO_FRAMEBUFFER_FRAMEBUFFER_DEFS_H_
