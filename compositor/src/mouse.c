#include "mouse.h"

#define MOUSE_WIDTH 10
#define MOUSE_HEIGHT 10

static uint32_t mouse_saved_pixels[MOUSE_WIDTH * MOUSE_HEIGHT];
static uint8_t mouse_saved = 0;
static uint32_t mouse_saved_x = 0;
static uint32_t mouse_saved_y = 0;
static mouse_t mouse;

static void compositor_save_mouse_pixels(gfx_context_t* ctx, uint32_t x, uint32_t y) {
    for (uint32_t x_off = 0; x_off < MOUSE_WIDTH; x_off++) {
        for (uint32_t y_off = 0; y_off < MOUSE_HEIGHT; y_off++) {
            if (y + y_off >= ctx->fb.height || y + y_off < 0)
                continue;

            if (x + x_off >= ctx->fb.width || x + x_off < 0)
                continue;

            mouse_saved_pixels[y_off * MOUSE_WIDTH + x_off] = ctx->pixels[(y + y_off) * ctx->fb.width + (x + x_off)];
        }
    }
    
    mouse_saved_x = x;
    mouse_saved_y = y;
    mouse_saved = 1;
}

static void compositor_redraw_saved_mouse_pixels(gfx_context_t* ctx) {
    for (uint32_t x_off = 0; x_off < MOUSE_WIDTH; x_off++) {
        for (uint32_t y_off = 0; y_off < MOUSE_HEIGHT; y_off++) {
            if (mouse_saved_y + y_off >= ctx->fb.height)
                continue;

            if (mouse_saved_x + x_off >= ctx->fb.width)
                continue;

            gfx_set_pixel(ctx, mouse_saved_x + x_off, mouse_saved_y + y_off, mouse_saved_pixels[y_off * MOUSE_WIDTH + x_off]);
        }
    }
    
    gfx_mark_dirty(ctx, mouse_saved_x, mouse_saved_y, MOUSE_WIDTH, MOUSE_HEIGHT);
    mouse_saved = 0;
}

static int32_t compositor_get_mouse_info(uint32_t input_fd, input_event_t* mouse_events) {
    input_event_t buffer[32];

    int32_t bytes_read = read(input_fd, (char*)buffer, sizeof(buffer));
    if (bytes_read < 0)
        return -1;
    
    uint32_t num_events = bytes_read / sizeof(input_event_t); 
    uint32_t num_mouse_events = 0; 
    
    for (uint32_t i = 0; i < num_events; i++) {
        input_event_t event = buffer[i];

        if (event.type == INPUT_EVENT_MOUSE || event.type == INPUT_EVENT_MOUSE_CLICK) {
            if (num_mouse_events >= 32)
                return num_mouse_events;

            mouse_events[num_mouse_events++] = event;
        }
    }

    
    return num_mouse_events;
}

static void compositor_render_mouse(gfx_context_t* ctx) {
    gfx_draw_rect(ctx, mouse.x, mouse.y, 10, 10, FB_WHITE);
}

