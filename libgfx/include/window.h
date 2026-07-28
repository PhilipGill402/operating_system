#ifndef INCLUDE_WINDOW_WINDOW_H_
#define INCLUDE_WINDOW_WINDOW_H_

#include <stdint.h>

#include <string.h>
#include <stdlib.h>

#include "gfx.h"
#include "gfx/rendering.h"

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

    gfx_context_t* ctx;
} window_t;

uint32_t window_create(char* title, uint32_t width, uint32_t height);
void window_destroy(uint32_t id);
void window_blit(gfx_context_t* ctx, uint32_t win_id);
gfx_context_t* window_get_context(uint32_t id);
int32_t window_hit_id(uint32_t x, uint32_t y);

extern window_t windows[MAX_WINDOWS];
extern int32_t focused_window;

#endif // !INCLUDE_WINDOW_WINDOW_H_
