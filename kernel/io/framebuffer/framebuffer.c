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

void framebuffer_putchar(char c, void* ctx) {
    (void)ctx;
    
    if (c == '\n') {
        framebuffer.y += 10;
        framebuffer.x = 5;

        return;
    } 


    framebuffer_draw_char(c, framebuffer.x, framebuffer.y, FB_WHITE, FB_BLACK);

    framebuffer.x += 8;

    if (framebuffer.x >= framebuffer.width) {
        framebuffer.x = 5;
        framebuffer.y += 10;
    }
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
    framebuffer.x = 5;
    framebuffer.y = 5;
    
    io_put_char = framebuffer_putchar;

    fs_node_t* img = resolve_path("/bin/test.bmp", fs_root);
    if (!img) {
        log_error("failed to find img\n");
        return 1;
    }

    uint8_t* buffer = kmalloc(25000);
    if (!buffer) {
        log_error("failed to allocate buffer\n");
        return 1;
    }
    
    int32_t bytes_read = fs_read(img, 0, 24630, buffer);
    framebuffer_draw_bitmap(buffer, 100, 100);
    
    return 1;
}

