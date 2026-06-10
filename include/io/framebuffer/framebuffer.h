#ifndef INCLUDE_IO_FRAMEBUFFER_H_
#define INCLUDE_IO_FRAMEBUFFER_H_

#include <stdint.h>

#include <log.h>

#include "io/io.h"
#include "io/framebuffer/framebuffer_defs.h"
#include "io/framebuffer/framebuffer_graphics.h"
#include "multiboot.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "io/font8x8.h"

int32_t framebuffer_init(multiboot_info_t* mbi);
uint32_t framebuffer_draw_char(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
uint32_t framebuffer_draw_string(char* str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void framebuffer_putchar(char c, void* ctx);
void framebuffer_draw_cursor();
void framebuffer_clear_cursor();

#endif
