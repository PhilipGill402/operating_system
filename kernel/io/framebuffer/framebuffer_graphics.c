#include "io/framebuffer/framebuffer_graphics.h"

fb_rect_t fb_dirty_objs[MAX_DIRTY_OBJS] = { 0 };
uint32_t dirty_count = 0;

uint32_t framebuffer_mark_dirty(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
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

uint32_t framebuffer_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
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

static void framebuffer_present_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (x + width >= framebuffer.width)
        width = framebuffer.width - x;
    if (y + width >= framebuffer.height)
        height = framebuffer.height - y;

    for (uint32_t row = 0; row < height; row++) {
        uint8_t* dst = framebuffer.addr + (y + row) * framebuffer.pitch + x * 4;
        uint8_t* src = (uint8_t*)framebuffer.backbuffer + ((y + row) * framebuffer.width + x) * 4;

        memcpy(dst, src, width * 4);
    }
}

void framebuffer_flush() {
    for (uint32_t i = 0; i < dirty_count; i++) {
        fb_rect_t rect = fb_dirty_objs[i];
        framebuffer_present_rect(rect.x, rect.y, rect.width, rect.height);
    }

    dirty_count = 0;
}

void framebuffer_clear(uint32_t color) {
    if (!framebuffer.backbuffer)
        return;

    uint32_t size = framebuffer.width * framebuffer.height;

    for (uint32_t off = 0; off < size; off++) {
        framebuffer.backbuffer[off] = color;
    }
}

uint32_t framebuffer_draw_bitmap(uint8_t* bytes, uint32_t x, uint32_t y) {
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
