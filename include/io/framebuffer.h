#ifndef INCLUDE_IO_FRAMEBUFFER_H_
#define INCLUDE_IO_FRAMEBUFFER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <log.h>

#include <arch/memory/paging.h>

#include "multiboot.h"
#include "memory/heap.h"
#include "memory_mapping.h"

#define FB_BLACK        0x00000000
#define FB_WHITE        0x00FFFFFF

#define FB_RED          0x00FF0000
#define FB_GREEN        0x0000FF00
#define FB_BLUE         0x000000FF

#define FB_YELLOW       0x00FFFF00
#define FB_CYAN         0x0000FFFF
#define FB_MAGENTA      0x00FF00FF

#define FB_GRAY         0x00808080
#define FB_DARK_GRAY    0x00404040
#define FB_LIGHT_GRAY   0x00C0C0C0

#define FB_ORANGE       0x00FFA500
#define FB_PURPLE       0x00800080
#define FB_BROWN        0x00A52A2A
#define FB_PINK         0x00FFC0CB



typedef struct {
    uint8_t bpp;
    uint32_t* backbuffer;
    uint8_t* addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
} framebuffer_t;

typedef struct fb_shared_buffer {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t size;

    uint32_t kernel_vaddr;

    uint32_t* frames;
    uint32_t frame_count;

    int32_t owner_pid;
} fb_shared_buffer_t;

uint32_t framebuffer_set_pixel_raw(uint32_t x, uint32_t y, uint32_t color);
void framebuffer_clear(uint32_t color);
int32_t framebuffer_init(multiboot_info_t* mbi);

extern framebuffer_t framebuffer;
extern fb_shared_buffer_t fb_shared_buffer;

#endif
