#ifndef LIBGFX_INCLUDE_GFX_H_
#define LIBGFX_INCLUDE_GFX_H_

#include <stdint.h>
#include <stddef.h>

#include <sys/fb.h>

#define MAX_DIRTY_RECTS  128

typedef struct {
    fb_info_t fb;
    uint32_t* pixels;

    gfx_rect_t dirty_rects[MAX_DIRTY_RECTS];
    uint32_t dirty_count;
    uint8_t dirty_fullscreen;
} gfx_context_t;

gfx_context_t* gfx_get_context(fb_info_t info);
void gfx_free_context(gfx_context_t* ctx, uint32_t* pixels);

#endif
