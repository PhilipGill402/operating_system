#include "gfx.h"

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

void gfx_free_context(gfx_context_t* ctx) {
    if (ctx->owns_pixels) {
        if (munmap(ctx->pixels, ctx->fb.size) < 0)
            return;
    }

    free(ctx);
}
