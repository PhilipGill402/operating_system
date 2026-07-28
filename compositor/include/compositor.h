#ifndef INCLUDE_COMPOSITOR_H_
#define INCLUDE_COMPOSITOR_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/events.h>
#include <gfx/gfx.h>
#include <gfx/window.h>

typedef struct {
    // graphics
    gfx_context_t* ctx;
    
    // tty
    uint32_t tty_pid;

    // mouse 
    uint32_t mouse_x;
    uint32_t mouse_y;

    // keyboard events
    uint32_t modifiers;
} compositor_t;

typedef struct {
    
} mouse_t;

#endif // !INCLUDE_COMPOSITOR_H_
