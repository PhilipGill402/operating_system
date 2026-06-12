#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/io.h>

#include <gfx/gfx.h>
#include <gfx/rendering.h>
#include <gfx/8x8font.h>

int main(void) {
    int32_t serial_fd = open("/dev/serial", O_WRONLY, 0); 
    int32_t input_fd = open("/dev/input", O_RDONLY, 0);
    
    while (1) {
        uint8_t* buffer = malloc(10*sizeof(input_event_t));
        uint32_t bytes_read = read(input_fd, buffer, 10*sizeof(input_event_t));

        input_event_t* events = (input_event_t*)buffer;
        
        write(serial_fd, &events[0].ch, 1);
    }
    

    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    
    uint32_t ret = gfx_draw_string(ctx, "WHATS UP\n", 100, 100, FB_WHITE, FB_BLACK);

    gfx_flush(ctx);
    
    gfx_free_context(ctx); 

    return 0;
}
