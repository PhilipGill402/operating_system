#include "gfx.h"

#include <stdio.h>

gfx_context_t* gfx_get_context(fb_info_t* info, uint32_t* pixels) {
    gfx_context_t* ctx = malloc(sizeof(gfx_context_t));
    if (!ctx)
        return NULL;

    if (!info)
        fb_info(&ctx->fb);
    else
        ctx->fb = *info;
    
    if (!pixels) {
        pixels = mmap(NULL, ctx->fb.size, PROT_READ | PROT_WRITE, MAP_FRAMEBUFFER, -1, 0);
        if (!pixels)
            return NULL;

        ctx->owns_pixels = 1;
    } else {
        ctx->owns_pixels = 0;
    }

    
    ctx->pixels = pixels;
    ctx->dirty_count = 0;
    ctx->dirty_fullscreen = 0;

    return ctx;
}

uint32_t gfx_flush(gfx_context_t* ctx) {
    for (uint32_t i = 0; i < ctx->dirty_count; i++) {
        gfx_rect_t rect = ctx->dirty_rects[i]; 
        fb_flush(rect.x, rect.y, rect.width, rect.height);
    }

    return 1;
}

void gfx_free_context(gfx_context_t* ctx) {
    if (ctx->owns_pixels) {
        if (munmap(ctx->pixels, ctx->fb.size) < 0)
            return;
    }

    free(ctx);
}
