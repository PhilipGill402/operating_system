/*
#ifndef INCLUDE_WINDOW_WINDOW_H_
#define INCLUDE_WINDOW_WINDOW_H_

#include <stdint.h>

#include <log.h>
#include <string.h>

#include "io/framebuffer/framebuffer.h"
#include "interrupts/mouse.h"

#define MAX_WINDOWS 32
#define WINDOW_TITLE_MAX 64

#define WINDOW_BORDER_SIZE 10
#define WINDOW_TITLE_SIZE 20

typedef struct window {
    uint32_t id;

    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;

    char title[WINDOW_TITLE_MAX];

    uint32_t bg_color;
    uint8_t visible;
    uint8_t focused;
    uint8_t in_use;

    uint32_t* buffer;
} window_t;

void window_init();
uint32_t window_create(char* title);
void window_destroy(uint32_t id);
void window_draw(uint32_t id);
void window_render();
int32_t window_hit_id(uint32_t x, uint32_t y);

#endif // !INCLUDE_WINDOW_WINDOW_H_
*/
