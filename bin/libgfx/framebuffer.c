#include "framebuffer.h"

/* --- BASIC RENDERING --- */
fb_rect_t fb_dirty_objs[MAX_DIRTY_OBJS] = { 0 };
uint32_t dirty_count = 0;

uint32_t framebuffer_set_pixel(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;
    
    framebuffer.backbuffer[y * framebuffer.width + x] = color;

    return 1;
}

uint32_t framebuffer_mark_dirty(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (x >= framebuffer.width)
        return 0;
    else if (y >= framebuffer.height)
        return 0;
    else if (dirty_count >= MAX_DIRTY_OBJS)
        return 0;

    if (x + width >= framebuffer.width)
        width = framebuffer.width - x;
    if (y + height >= framebuffer.height)
        height = framebuffer.height - y;

    fb_rect_t rect = {
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };
    
    fb_dirty_objs[dirty_count++] = rect;

    return 1;
}

/* --- TEXT RENDERING --- */

void framebuffer_scroll(gfx_context_t* ctx) {
    
    // skip first line
    uint8_t* copy = framebuffer.addr + (framebuffer.pitch * FONT_HEIGHT);
    uint32_t len = (framebuffer.height - FONT_HEIGHT) * framebuffer.pitch;

    //get last line
    uint8_t* last_line = framebuffer.addr + (framebuffer.pitch * (framebuffer.height - FONT_HEIGHT));

    memmove(framebuffer.addr, copy, len);
    memset(last_line, 0, framebuffer.pitch * FONT_HEIGHT);

    framebuffer.y = framebuffer.height - FONT_HEIGHT;
}

uint32_t framebuffer_draw_char(gfx_context_t* ctx, char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    uint8_t* font_char = font8x8_basic[(uint8_t)c];

    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {
            uint32_t color;

            if (font_char[row] & (1 << col)) {
                color = fg;
            } else {
                color = bg;
            }

            if (!framebuffer_set_pixel(x + col, y + row, color))
                return 0;
        }
    }

    framebuffer_mark_dirty(x, y, FONT_WIDTH, FONT_HEIGHT);

    return 1;
}

uint32_t framebuffer_draw_string(gfx_context_t* ctx, char* str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    uint32_t x_off = 0;

    while (*str) {
        if (!framebuffer_draw_char(*str, x + x_off, y, fg, bg))
            return 0;

        str++;
        x_off += FONT_WIDTH;
    } 

    return 1;
}

/* --- SHAPE AND IMAGE RENDERING --- */

uint32_t framebuffer_draw_rect(gfx_context_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    if (width == 0 || height == 0)
        return 0;

    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;

    if (x + width > framebuffer.width)
        width = framebuffer.width - x;

    if (y + height > framebuffer.height)
        height = framebuffer.height - y;

    for (uint32_t x_off = 0; x_off < width; x_off++) {
        for (uint32_t y_off = 0; y_off < height; y_off++) {
            if (!framebuffer_set_pixel(x + x_off, y + y_off, color))
                return 0;
        }
    }

    framebuffer_mark_dirty(x, y, width, height);

    return 1;
}

uint32_t framebuffer_draw_bitmap(gfx_context_t* ctx, uint8_t* bytes, uint32_t x, uint32_t y) {
    bmp_file_header_t* header = (bmp_file_header_t*)bytes;
    bmp_info_header_t* info = (bmp_info_header_t*)(bytes + sizeof(bmp_file_header_t));

    // make sure signature is correct 
    if (header->signature != 0x4D42)
        return 0;

    if (info->compression)
        return 0;
    
    uint8_t* bmp = bytes + header->pixel_offset;

    for (uint32_t x_off = 0; x_off < info->width; x_off++) {
        for (uint32_t y_off = 0; y_off < info->height; y_off++) {
            uint32_t bmp_offset = (y_off * info->width + x_off) * (info->bits_per_pixel / 8);

            uint8_t b = bmp[bmp_offset];
            uint8_t g = bmp[bmp_offset + 1];
            uint8_t r = bmp[bmp_offset + 2];
            uint32_t color = (r << 16) | (g << 8) | b;
            framebuffer_set_pixel(x + x_off, y + y_off, color);
        }
    }

    return 1;
}
