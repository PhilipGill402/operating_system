#ifndef LIBC_INCLUDE_SYS_EVENTS_H_
#define LIBC_INCLUDE_SYS_EVENTS_H_

#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_CENTER 4

typedef enum {
    INPUT_EVENT_KEY,
    INPUT_EVENT_MOUSE,
    INPUT_EVENT_MOUSE_CLICK,
} event_type_t;

typedef struct {
    uint32_t type;
    uint32_t keycode;
    char ch;
    uint8_t pressed;
    uint8_t modifiers;
} input_event_t;

#endif // !LIBC_INCLUDE_SYS_EVENTS_H_
