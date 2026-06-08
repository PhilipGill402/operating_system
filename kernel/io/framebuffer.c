#include "io/framebuffer.h"

static framebuffer_t framebuffer;

static inline uint32_t fb_get_color(fb_color_t color) {
    return *(uint32_t*)(&color);
}

static fb_color_t fb_color_create(uint8_t r, uint8_t g, uint8_t b) {
    fb_color_t color = {
        .unused = 0,
        .r = r,
        .g = g,
        .b = b,
    };

    return color;
}

static uint32_t framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;

    uint32_t* pixel = (uint32_t*)(framebuffer.addr + y * framebuffer.pitch + x * 4);
    *pixel = color;

    return 1;
}

static uint32_t framebuffer_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t x_off = 0; x_off < width; x_off++) {
        for (uint32_t y_off = 0; y_off < height; y_off++) {
            if (!framebuffer_set_pixel(x + x_off, y + y_off, color))
                return 0;
        }
    }

    return 1;
}

static uint32_t framebuffer_draw_char(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
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

    return 1;
}

void framebuffer_init(multiboot_info_t* mbi) {
    uint64_t fb_phys = mbi->framebuffer_addr;
    
    if (fb_phys > 0xFFFFFFFF) {
        log_error("framebuffer above 4 GB not supported\n");
        return;
    }

    uint32_t fb_size = mbi->framebuffer_pitch * mbi->framebuffer_height;

    for (uint32_t offset = 0; offset < fb_size; offset += PAGE_SIZE) {
        map_page(FRAMEBUFFER_VIRT + offset, fb_phys + offset, PAGE_PRESENT | PAGE_WRITE);
    }

    framebuffer.addr = (uint8_t*)FRAMEBUFFER_VIRT;
    framebuffer.width = mbi->framebuffer_width;
    framebuffer.height = mbi->framebuffer_height;
    framebuffer.pitch = mbi->framebuffer_pitch;
    framebuffer.bpp = mbi->framebuffer_bpp;
    
    if (!framebuffer_draw_char('a', 100, 100, FB_WHITE, FB_BLACK))
        log_error("couln't draw char\n");
}

