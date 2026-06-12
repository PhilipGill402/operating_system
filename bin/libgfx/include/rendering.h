#ifndef LIBC_INCLUDE_FRAMEBUFFER_H_
#define LIBC_INCLUDE_FRAMEBUFFER_H_

#include <stdint.h>

#include <sys/bmp.h>

#include "gfx.h"
#include "8x8font.h"

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
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t unused;
} __attribute__((packed)) fgx_color_t;


/* --- Basic Rendering --- */
uint32_t gfx_set_pixel(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t color);
uint32_t gfx_mark_dirty(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/* --- Rendering Text --- */
uint32_t gfx_draw_char(gfx_context_t* ctx, char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void gfx_put_char(gfx_context_t* ctx, char c, void* args);
uint32_t gfx_draw_string(gfx_context_t* ctx, char* str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);

/* --- Rendering Shapes --- */
uint32_t gfx_draw_rect(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);

#endif // !LIBC_INCLUDE_FRAMEBUFFER_H_
