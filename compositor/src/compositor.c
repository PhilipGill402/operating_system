#include <stdio.h>
#include <errno.h>
#include <gfx/gfx.h>
#include <sys/events.h>
#include "window.h"

static int32_t compositor_get_mouse_info(uint32_t input_fd, input_event_t* mouse_events) {
    input_event_t buffer[256];

    int32_t bytes_read = read(input_fd, (char*)buffer, sizeof(buffer));
    if (bytes_read < 0)
        return -1;
    
    uint32_t num_events = bytes_read / sizeof(input_event_t); 
    uint32_t num_mouse_events = 0; 
    
    for (uint32_t i = 0; i < num_events; i++) {
        input_event_t event = buffer[i];

        if (event.type == INPUT_EVENT_MOUSE || event.type == INPUT_EVENT_MOUSE_CLICK) {
            mouse_events[num_mouse_events++] = event;
        }
    }

    
    return num_mouse_events;
}

static void compositor_render_mouse(gfx_context_t* ctx, input_event_t event) {
    return;
}

int main() {
    close(1); // close stdout
    int32_t serial_fd = open("/dev/serial", O_WRONLY, 0); // open serial for output
    if (serial_fd < 0) {
        return -1; 
    }

    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    if (!ctx) {
        return -1;
    }
    
    errno = 0;
    int32_t input_fd = open("/dev/input", O_RDONLY, 0);
    if (input_fd < 0) {
        perror("open");
        return -1;
    }

    window_init();

    window_create(ctx, "test");

    while(1) {
        input_event_t mouse_events[32];
        int32_t num_events = compositor_get_mouse_info(input_fd, mouse_events);
        
        if (num_events < 0)
            continue;
        
        for (uint32_t i = 0; i < num_events; i++)
            input_event_t event = mouse_events[i];

        compositor_render_mouse(ctx, event);

        window_render(ctx);
    }


    gfx_free_context(ctx);
}
