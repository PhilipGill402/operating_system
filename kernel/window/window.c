#include "window/window.h"

static window_t windows[MAX_WINDOWS] = { 0 };
static int32_t focused_window = -1;

static int32_t window_get_index(uint32_t id) {
    int32_t idx = -1;
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == id) {
            idx = i; 
            break;
        }
    }
    
    return idx;
}

static void window_draw_border(uint32_t id) {
    int32_t idx = window_get_index(id);
    if (idx == -1)
        return;

    window_t window = windows[idx];
    
    // draws top border
    framebuffer_draw_rect(window.x, window.y - WINDOW_TITLE_SIZE , window.width, WINDOW_TITLE_MAX, FB_GRAY);

    // draws 'x'
    framebuffer_draw_char('x', window.x + 10, window.y - 15, FB_RED, FB_GRAY);

    // draw title
    framebuffer_draw_string(window.title, window.x + 50, window.y - 15, FB_BLACK, FB_GRAY);
}

static void window_draw_all() {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].in_use)
            window_draw(i);
    }

    mouse_update_screen();

    framebuffer_flush();
}

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

    windows[idx].height = 300;
    windows[idx].width = 600;
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
    int32_t idx = window_get_index(id);

    if (idx == -1)
        return;

    window_t window = windows[idx];
    
    window_draw_border(id);

    // draw window data
    for (uint32_t x_off = 0; x_off < window.width; x_off++) {
        for (uint32_t y_off = 0; y_off < window.height; y_off++) {
            uint32_t pixel = window.buffer[y_off * window.width + x_off];
            framebuffer_set_pixel(window.x + x_off, window.y + y_off, pixel);
        }
    }

    framebuffer_mark_dirty(window.x, window.y, window.width, window.height);
}


void window_render() {
    framebuffer_clear(FB_BLACK);
    window_draw_all();
    framebuffer_flush();
    mouse_draw();
}

int32_t window_hit_id(uint32_t x, uint32_t y) {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        window_t window = windows[i];

        if (!window.in_use || !window.visible)
            continue;

        uint32_t win_x = window.x;
        uint32_t win_y = window.y - WINDOW_TITLE_SIZE;
        uint32_t win_width = window.width;
        uint32_t win_height = window.height + WINDOW_TITLE_SIZE;

        if (x >= win_x && x <= win_x + win_width && y >= win_y && y <= win_y + win_height)
            return i;
    }

    return -1;
}


