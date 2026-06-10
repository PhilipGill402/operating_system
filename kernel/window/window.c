#include "window/window.h"

static window_t windows[MAX_WINDOWS] = { 0 };
static int32_t focused_window = -1;

void window_init() {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        windows[i].id = i;
    }
}

uint32_t window_create(char* title) {
    int32_t idx = -1;

    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (!windows[i].in_use) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        log_error("couln't find a free window\n");
        return MAX_WINDOWS + 1;
    }

    windows[idx].height = 100;
    windows[idx].width = 100;
    windows[idx].y = (framebuffer.height / 2) - (windows[idx].height / 2);
    windows[idx].x = (framebuffer.width / 2) - (windows[idx].width / 2);

    strncpy(windows[idx].title, title, WINDOW_TITLE_MAX);

    windows[idx].bg_color = FB_WHITE;
    windows[idx].visible = 1;

    windows[idx].focused = 1;
    focused_window = windows[idx].id;
    
    windows[idx].in_use = 1;
    
    windows[idx].buffer = kmalloc(windows[idx].height * windows[idx].width * sizeof(uint32_t));
    
    uint32_t pixel_count = windows[idx].height * windows[idx].width;
    for (uint32_t i = 0; i < pixel_count; i++) {
        windows[idx].buffer[i] = windows[idx].bg_color;
    }

    return windows[idx].id;
}

void window_destroy(uint32_t id) {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == id) {
            windows[i].in_use = 0;
            kfree(windows[i].buffer);
            return;
        }
    }
}

void window_draw(uint32_t id) {
    int32_t idx = -1;
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == id) {
            idx = i; 
            break;
        }
    }

    if (idx == -1)
        return;

    window_t window = windows[idx];

    for (uint32_t x_off = 0; x_off < window.width; x_off++) {
        for (uint32_t y_off = 0; y_off < window.height; y_off++) {
            uint32_t pixel = window.buffer[y_off * window.width + x_off];
            framebuffer_set_pixel(window.x + x_off, window.y + y_off, pixel);
        }
    }

    framebuffer_mark_dirty(window.x, window.y, window.width, window.height);
}


