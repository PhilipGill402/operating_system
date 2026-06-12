#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#include <gfx/gfx.h>
#include <gfx/rendering.h>
#include <gfx/8x8font.h>

int main(void) {
    int32_t serial_fd = open("/dev/serial", O_WRONLY, 0);
    char* msg = "hello world\n"; 

    write(serial_fd, msg, strlen(msg));

    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    
    uint32_t ret = gfx_draw_string(ctx, "WHATS UP\n", 100, 100, FB_WHITE, FB_BLACK);

    gfx_flush(ctx);
    
    gfx_free_context(ctx); 

    return 0;
}
