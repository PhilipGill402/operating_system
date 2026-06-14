#ifndef INCLUDE_INTERRUPTS_EVENTS_H_
#define INCLUDE_INTERRUPTS_EVENTS_H_

#include <stdint.h>

#include <queue.h>

#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_CENTER 4

typedef enum {
    INPUT_EVENT_KEY,
    INPUT_EVENT_MOUSE,
    INPUT_EVENT_MOUSE_CLICK,
} event_type_t;

typedef struct {
    event_type_t type;
    
    uint32_t keycode;
    char ch;
    uint8_t pressed;
    uint8_t modifiers;

    int32_t mouse_x;
    int32_t mouse_y;
    int32_t mouse_dx;
    int32_t mouse_dy;
    uint32_t mouse_buttons;
} input_event_t;

void events_init();

extern queue_t input_buffer;

#endif
