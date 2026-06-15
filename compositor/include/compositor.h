#ifndef INCLUDE_COMPOSITOR_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/events.h>
#include <gfx/gfx.h>
#include "window.h"

typedef struct {
    uint32_t x;
    uint32_t y;
} mouse_t;

#endif // !INCLUDE_COMPOSITOR_H_
