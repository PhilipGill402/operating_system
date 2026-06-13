#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <gtx/gtx.c>

#define MAX_BUFFER_LENGTH 256

int main() {
    uint32_t input_fd = open("/dev/input", O_RDONLY, 0);
    uint32_t ptm_fd = open("/dev/ptm", O_RDWR, 0);

    gtx_context_t* ctx = gtx_get_context(NULL, NULL);

    while (1) {
        input_event_t* input_event_buffer = malloc(sizeof(input_event_t) * 10); 
        uint32_t event_bytes_read = read(input_fd, input_event_buffer, sizeof(input_event_t) * 10);
        uint32_t events_read = event_bytes_read / sizeof(input_event_t);
        
        uint8_t* bytes = malloc(10);
        uint8_t num_chars = 0;
        for (uint32_t i = 0; i < events_read; i++) {
            input_event_t event = input_event_buffer[i];

            if (event.ch) {
                bytes[num_chars++] = event.ch; 
            }
        }

        uint32_t bytes_written = write(ptm_fd, bytes, num_chars);
        
        uint32_t bytes_read = read(ptm_fd, bytes, 10);
        bytes[bytes_read] = '\0';

        gfx_draw_string(ctx, bytes, 100, 100, FB_WHITE, FB_BLACK);

        free(bytes);
        free(input_event_buffer);
    }

    get_free_context(ctx);
}
