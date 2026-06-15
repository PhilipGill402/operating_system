#include "window.h"

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

static void window_draw_border(gfx_context_t* ctx, uint32_t id) {
    int32_t idx = window_get_index(id);
    if (idx == -1)
        return;

    window_t window = windows[idx];
    
    // draws top border
    gfx_draw_rect(ctx, window.x, window.y - WINDOW_TITLE_SIZE , window.width, WINDOW_TITLE_MAX, FB_GRAY);

    // draws 'x'
    gfx_draw_char(ctx, 'x', window.x + 10, window.y - 15, FB_RED, FB_GRAY);

    // draw title
    gfx_draw_string(ctx, window.title, window.x + 50, window.y - 15, FB_BLACK, FB_GRAY);
}

static void window_draw_all(gfx_context_t* ctx) {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].in_use)
            window_draw(ctx, i);
    }
}

void window_init() {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        windows[i].id = i;
    }
}

uint32_t window_create(gfx_context_t* ctx, char* title) {
    int32_t idx = -1;

    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (!windows[i].in_use) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        return MAX_WINDOWS + 1;
    }

    windows[idx].height = 300;
    windows[idx].width = 600;
    windows[idx].y = (ctx->fb.height / 2) - (windows[idx].height / 2);
    windows[idx].x = (ctx->fb.width / 2) - (windows[idx].width / 2);

    strncpy(windows[idx].title, title, WINDOW_TITLE_MAX);

    windows[idx].bg_color = FB_WHITE;
    windows[idx].visible = 1;

    windows[idx].focused = 1;
    focused_window = windows[idx].id;
    
    windows[idx].in_use = 1;
    
    windows[idx].buffer = malloc(windows[idx].height * windows[idx].width * sizeof(uint32_t));
    
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
            free(windows[i].buffer);
            return;
        }
    }
}

void window_draw(gfx_context_t* ctx, uint32_t id) {
    int32_t idx = window_get_index(id);

    if (idx == -1)
        return;

    window_t window = windows[idx];
    
    window_draw_border(ctx, id);

    // draw window data
    for (uint32_t x_off = 0; x_off < window.width; x_off++) {
        for (uint32_t y_off = 0; y_off < window.height; y_off++) {
            uint32_t pixel = window.buffer[y_off * window.width + x_off];
            gfx_set_pixel(ctx, window.x + x_off, window.y + y_off, pixel);
        }
    }

    gfx_mark_dirty(ctx, window.x, window.y, window.width, window.height);
}


void window_render(gfx_context_t* ctx) {
    window_draw_all(ctx);
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
