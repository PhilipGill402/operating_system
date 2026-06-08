#include "io/framebuffer/framebuffer_graphics.h"

uint32_t framebuffer_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t x_off = 0; x_off < width; x_off++) {
        for (uint32_t y_off = 0; y_off < height; y_off++) {
            if (!framebuffer_set_pixel(x + x_off, y + y_off, color))
                return 0;
        }
    }

    return 1;
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
