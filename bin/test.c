#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#include <gfx/gfx.h>
#include <gfx/rendering.h>
#include <gfx/8x8font.h>

int main(void) {
    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    
    uint32_t ret = gfx_draw_string(ctx, "Hello world\n", 100, 100, FB_WHITE, FB_BLACK);

    fb_flush(100, 100, 100, 100);
    
    gfx_free_context(ctx); 

    return 0;
}
