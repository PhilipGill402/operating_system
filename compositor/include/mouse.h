#ifndef COMPOSITOR_INCLUDE_MOUSE_H_
#define COMPOSITOR_INCLUDE_MOUSE_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/events.h>
#include <gfx/gfx.h>
#include "window.h"

typedef struct {
    uint32_t x;
    uint32_t y;
} mouse_t;

#endif
