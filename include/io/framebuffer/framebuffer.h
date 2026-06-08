#ifndef INCLUDE_IO_FRAMEBUFFER_H_
#define INCLUDE_IO_FRAMEBUFFER_H_

#include <stdint.h>

#include <log.h>

#include "io/io.h"
#include "io/framebuffer/framebuffer_defs.h"
#include "io/framebuffer/framebuffer_graphics.h"
#include "multiboot.h"
#include "memory/paging.h"
#include "io/font8x8.h"

int32_t framebuffer_init(multiboot_info_t* mbi);
void framebuffer_putchar(char c, void* ctx);
void framebuffer_draw_cursor();
void framebuffer_clear_cursor();

#endif
