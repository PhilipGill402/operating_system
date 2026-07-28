#include "window.h"

#include <stdio.h>

window_t windows[MAX_WINDOWS] = { 0 };
uint32_t num_windows = 0;
int32_t focused_window = -1;

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


uint32_t window_create(char* title, uint32_t width, uint32_t height) {
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
    
    windows[idx].id = num_windows++;
    windows[idx].height = height;
    windows[idx].width = width;
    windows[idx].y = 0;
    windows[idx].x = 0;

    strncpy(windows[idx].title, title, WINDOW_TITLE_MAX);

    windows[idx].bg_color = FB_WHITE;
    windows[idx].visible = 1;

    windows[idx].focused = 1;
    focused_window = windows[idx].id;
    
    windows[idx].in_use = 1;
    
    fb_info_t info = {
        .width = width,
        .height = height,
        .pitch = width * sizeof(uint32_t),
        .bpp = sizeof(uint32_t),
        .size = width * height * sizeof(uint32_t)
    };
    
    uint32_t* buffer = malloc(info.size);
    memset(buffer, windows[idx].bg_color, info.size);

    windows[idx].ctx = gfx_get_context(&info, buffer);
    printf("CREATE windows addr: %x\n", windows);
    printf("GIVEN: %d\n", windows[idx].id);
    printf("GIVEN: %x\n", windows[idx].ctx);
    gfx_mark_dirty(windows[idx].ctx, windows[idx].x, windows[idx].y, width, height); 

    return windows[idx].id;
}

void window_destroy(uint32_t id) {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == id) {
            windows[i].in_use = 0;

            // we own this so we must free it
            free(windows[i].ctx->pixels);
            gfx_free_context(windows[i].ctx); 
            return;
        }
    }
}

void window_blit(gfx_context_t* ctx, uint32_t win_id) {
    int32_t idx = window_get_index(win_id);
    if (idx == -1)
        return;
    
    window_t window = windows[idx];
    printf("BLIT windows addr: %x\n", windows);
    printf("IDX: %d\n", window.id); 
    printf("ADDR: %x\n", window.ctx); 
    if (!window.ctx)
        return;
    
    for (uint32_t i = 0; i < window.ctx->dirty_count; i++) {
        gfx_rect_t rect = window.ctx->dirty_rects[i];
        
        uint32_t max_x = rect.width;
        uint32_t max_y = rect.height;

        if (rect.x + rect.width > ctx->fb.width)
            max_x = ctx->fb.width - rect.x;

        if (rect.y + rect.height > ctx->fb.height)
            max_y = ctx->fb.height - rect.y;
    
        for (uint32_t y_off = 0; y_off < max_y; y_off++) {
            for (uint32_t x_off = 0; x_off < max_x; x_off++) {
                uint32_t win_x = rect.x + x_off;
                uint32_t win_y = rect.y + y_off;

                uint32_t pixel = window.ctx->pixels[win_y * window.width + win_x];
                gfx_set_pixel(ctx, window.x + win_x, window.y + win_y, pixel);
            }
        }

        gfx_mark_dirty(ctx, window.x + rect.x, window.y + rect.y, max_x, max_y);
    }
}

gfx_context_t* window_get_context(uint32_t id) {
    int32_t idx = window_get_index(id);
    if (idx == -1)
        return NULL;

    return windows[idx].ctx;
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
