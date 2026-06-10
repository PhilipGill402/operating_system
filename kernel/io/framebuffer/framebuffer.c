#include "io/framebuffer/framebuffer.h"

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

uint32_t framebuffer_draw_char(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
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

uint32_t framebuffer_draw_string(char* str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    uint32_t x_off = 0;

    while (*str) {
        if (!framebuffer_draw_char(*str, x + x_off, y, fg, bg))
            return 0;

        str++;
        x_off += FONT_WIDTH;
    } 

    return 1;
}

static void framebuffer_scroll() {
    
    // skip first line
    uint8_t* copy = framebuffer.addr + (framebuffer.pitch * FONT_HEIGHT);
    uint32_t len = (framebuffer.height - FONT_HEIGHT) * framebuffer.pitch;

    //get last line
    uint8_t* last_line = framebuffer.addr + (framebuffer.pitch * (framebuffer.height - FONT_HEIGHT));

    memmove(framebuffer.addr, copy, len);
    memset(last_line, 0, framebuffer.pitch * FONT_HEIGHT);

    framebuffer.y = framebuffer.height - FONT_HEIGHT;
}

void framebuffer_draw_cursor() {
    framebuffer_draw_rect(framebuffer.x, framebuffer.y, FONT_WIDTH, FONT_HEIGHT, FB_WHITE);
}

void framebuffer_clear_cursor() {
    framebuffer_draw_rect(framebuffer.x, framebuffer.y, FONT_WIDTH, FONT_HEIGHT, FB_BLACK);
}

void framebuffer_putchar(char c, void* ctx) {
    (void)ctx;
    framebuffer_clear_cursor();

    if (c == '\n') {
        framebuffer.y += FONT_HEIGHT;
        framebuffer.x = FB_PADDING;
        if (framebuffer.y >= framebuffer.height - FONT_HEIGHT) {
            framebuffer_scroll();
        }
    } else if (c == '\t') {
        framebuffer.x = (framebuffer.x + 64) & ~(64 - 8);
    } else if (c == '\b' && framebuffer.x >= FONT_WIDTH + FB_PADDING) {
        framebuffer.x -= FONT_WIDTH;
        framebuffer_draw_char(' ', framebuffer.x, framebuffer.y, FB_WHITE, FB_BLACK);
    } else if (c == '\r') {
        framebuffer.x = FB_PADDING;
    } else if (c >= ' ') {
        framebuffer_draw_char(c, framebuffer.x, framebuffer.y, FB_WHITE, FB_BLACK);
        framebuffer.x += FONT_WIDTH;

        if (framebuffer.x + FONT_WIDTH >= framebuffer.width) {
            framebuffer.x = FB_PADDING;
            framebuffer.y += FONT_HEIGHT;
            
            if (framebuffer.y >= framebuffer.height) {
                framebuffer_scroll();
            }
        }
    }
    
    if (cursor_on) {
        framebuffer_draw_cursor();
    }
    
    framebuffer_flush();
    
}

int32_t framebuffer_init(multiboot_info_t* mbi) {
    uint64_t fb_phys = mbi->framebuffer_addr;
    
    if (fb_phys > 0xFFFFFFFF) {
        log_error("framebuffer above 4 GB not supported\n");
        return 0;
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
    framebuffer.x = FB_PADDING;
    framebuffer.y = FB_PADDING;
    framebuffer.backbuffer = kzmalloc(framebuffer.width * framebuffer.height * sizeof(uint32_t));
    
    if (!framebuffer.backbuffer)
        return 0;

    framebuffer_clear(FB_BLACK);

    io_put_char = framebuffer_putchar;

    cursor_on = 1;
    
    framebuffer_initialized = 1;
    
    return 1;
}

