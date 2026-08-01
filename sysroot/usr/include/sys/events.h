#ifndef LIBC_INCLUDE_SYS_EVENTS_H_
#define LIBC_INCLUDE_SYS_EVENTS_H_

#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_CENTER 4

#define KEY_MOD_SHIFT 0x01
#define KEY_MOD_CTRL  0x02
#define KEY_MOD_ALT   0x04
#define KEY_MOD_CAPS  0x08

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

#endif // !LIBC_INCLUDE_SYS_EVENTS_H_
