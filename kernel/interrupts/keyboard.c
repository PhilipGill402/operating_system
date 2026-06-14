#include "interrupts/keyboard.h"

queue_t input_buffer;
static volatile keyboard_modifiers = 0;

static const keycode_t scancode_set1_map[128] = {
    [0x00] = KEY_NONE,

    [0x01] = KEY_ESCAPE,
    [0x02] = KEY_1,
    [0x03] = KEY_2,
    [0x04] = KEY_3,
    [0x05] = KEY_4,
    [0x06] = KEY_5,
    [0x07] = KEY_6,
    [0x08] = KEY_7,
    [0x09] = KEY_8,
    [0x0A] = KEY_9,
    [0x0B] = KEY_0,

    [0x0C] = KEY_MINUS,
    [0x0D] = KEY_EQUAL,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = KEY_TAB,

    [0x10] = KEY_Q,
    [0x11] = KEY_W,
    [0x12] = KEY_E,
    [0x13] = KEY_R,
    [0x14] = KEY_T,
    [0x15] = KEY_Y,
    [0x16] = KEY_U,
    [0x17] = KEY_I,
    [0x18] = KEY_O,
    [0x19] = KEY_P,

    [0x1A] = KEY_LEFT_BRACKET,
    [0x1B] = KEY_RIGHT_BRACKET,
    [0x1C] = KEY_ENTER,
    [0x1D] = KEY_LEFT_CTRL,

    [0x1E] = KEY_A,
    [0x1F] = KEY_S,
    [0x20] = KEY_D,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,

    [0x27] = KEY_SEMICOLON,
    [0x28] = KEY_APOSTROPHE,
    [0x29] = KEY_GRAVE,

    [0x2A] = KEY_LEFT_SHIFT,
    [0x2B] = KEY_BACKSLASH,

    [0x2C] = KEY_Z,
    [0x2D] = KEY_X,
    [0x2E] = KEY_C,
    [0x2F] = KEY_V,
    [0x30] = KEY_B,
    [0x31] = KEY_N,
    [0x32] = KEY_M,

    [0x33] = KEY_COMMA,
    [0x34] = KEY_DOT,
    [0x35] = KEY_SLASH,

    [0x36] = KEY_RIGHT_SHIFT,
    [0x37] = KEY_KP_STAR,
    [0x38] = KEY_LEFT_ALT,
    [0x39] = KEY_SPACE,
    [0x3A] = KEY_CAPS_LOCK,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,

    [0x45] = KEY_NUM_LOCK,
    [0x46] = KEY_SCROLL_LOCK,

    [0x47] = KEY_KP_7,
    [0x48] = KEY_KP_8,
    [0x49] = KEY_KP_9,
    [0x4A] = KEY_KP_MINUS,
    [0x4B] = KEY_KP_4,
    [0x4C] = KEY_KP_5,
    [0x4D] = KEY_KP_6,
    [0x4E] = KEY_KP_PLUS,
    [0x4F] = KEY_KP_1,
    [0x50] = KEY_KP_2,
    [0x51] = KEY_KP_3,
    [0x52] = KEY_KP_0,
    [0x53] = KEY_KP_DOT,

    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};

static const keycode_t scancode_set1_e0_map[128] = {
    [0x1C] = KEY_KP_ENTER,
    [0x1D] = KEY_RIGHT_CTRL,

    [0x35] = KEY_KP_SLASH,
    [0x38] = KEY_RIGHT_ALT,

    [0x47] = KEY_HOME,
    [0x48] = KEY_ARROW_UP,
    [0x49] = KEY_PAGE_UP,

    [0x4B] = KEY_ARROW_LEFT,
    [0x4D] = KEY_ARROW_RIGHT,

    [0x4F] = KEY_END,
    [0x50] = KEY_ARROW_DOWN,
    [0x51] = KEY_PAGE_DOWN,
    [0x52] = KEY_INSERT,
    [0x53] = KEY_DELETE,
};

static const char key_ascii_normal[KEY_MAX] = {
    [KEY_1] = '1',
    [KEY_2] = '2',
    [KEY_3] = '3',
    [KEY_4] = '4',
    [KEY_5] = '5',
    [KEY_6] = '6',
    [KEY_7] = '7',
    [KEY_8] = '8',
    [KEY_9] = '9',
    [KEY_0] = '0',

    [KEY_MINUS] = '-',
    [KEY_EQUAL] = '=',
    [KEY_BACKSPACE] = '\b',
    [KEY_TAB] = '\t',

    [KEY_Q] = 'q',
    [KEY_W] = 'w',
    [KEY_E] = 'e',
    [KEY_R] = 'r',
    [KEY_T] = 't',
    [KEY_Y] = 'y',
    [KEY_U] = 'u',
    [KEY_I] = 'i',
    [KEY_O] = 'o',
    [KEY_P] = 'p',

    [KEY_LEFT_BRACKET] = '[',
    [KEY_RIGHT_BRACKET] = ']',
    [KEY_ENTER] = '\n',

    [KEY_A] = 'a',
    [KEY_S] = 's',
    [KEY_D] = 'd',
    [KEY_F] = 'f',
    [KEY_G] = 'g',
    [KEY_H] = 'h',
    [KEY_J] = 'j',
    [KEY_K] = 'k',
    [KEY_L] = 'l',

    [KEY_SEMICOLON] = ';',
    [KEY_APOSTROPHE] = '\'',
    [KEY_GRAVE] = '`',
    [KEY_BACKSLASH] = '\\',

    [KEY_Z] = 'z',
    [KEY_X] = 'x',
    [KEY_C] = 'c',
    [KEY_V] = 'v',
    [KEY_B] = 'b',
    [KEY_N] = 'n',
    [KEY_M] = 'm',

    [KEY_COMMA] = ',',
    [KEY_DOT] = '.',
    [KEY_SLASH] = '/',

    [KEY_SPACE] = ' ',

    [KEY_KP_STAR] = '*',
    [KEY_KP_MINUS] = '-',
    [KEY_KP_PLUS] = '+',
    [KEY_KP_DOT] = '.',

    [KEY_KP_0] = '0',
    [KEY_KP_1] = '1',
    [KEY_KP_2] = '2',
    [KEY_KP_3] = '3',
    [KEY_KP_4] = '4',
    [KEY_KP_5] = '5',
    [KEY_KP_6] = '6',
    [KEY_KP_7] = '7',
    [KEY_KP_8] = '8',
    [KEY_KP_9] = '9',

    [KEY_KP_ENTER] = '\n',
    [KEY_KP_SLASH] = '/',
};

static const char key_ascii_shift[KEY_MAX] = {
    [KEY_1] = '!',
    [KEY_2] = '@',
    [KEY_3] = '#',
    [KEY_4] = '$',
    [KEY_5] = '%',
    [KEY_6] = '^',
    [KEY_7] = '&',
    [KEY_8] = '*',
    [KEY_9] = '(',
    [KEY_0] = ')',

    [KEY_MINUS] = '_',
    [KEY_EQUAL] = '+',
    [KEY_BACKSPACE] = '\b',
    [KEY_TAB] = '\t',

    [KEY_Q] = 'Q',
    [KEY_W] = 'W',
    [KEY_E] = 'E',
    [KEY_R] = 'R',
    [KEY_T] = 'T',
    [KEY_Y] = 'Y',
    [KEY_U] = 'U',
    [KEY_I] = 'I',
    [KEY_O] = 'O',
    [KEY_P] = 'P',

    [KEY_LEFT_BRACKET] = '{',
    [KEY_RIGHT_BRACKET] = '}',
    [KEY_ENTER] = '\n',

    [KEY_A] = 'A',
    [KEY_S] = 'S',
    [KEY_D] = 'D',
    [KEY_F] = 'F',
    [KEY_G] = 'G',
    [KEY_H] = 'H',
    [KEY_J] = 'J',
    [KEY_K] = 'K',
    [KEY_L] = 'L',

    [KEY_SEMICOLON] = ':',
    [KEY_APOSTROPHE] = '"',
    [KEY_GRAVE] = '~',
    [KEY_BACKSLASH] = '|',

    [KEY_Z] = 'Z',
    [KEY_X] = 'X',
    [KEY_C] = 'C',
    [KEY_V] = 'V',
    [KEY_B] = 'B',
    [KEY_N] = 'N',
    [KEY_M] = 'M',

    [KEY_COMMA] = '<',
    [KEY_DOT] = '>',
    [KEY_SLASH] = '?',

    [KEY_SPACE] = ' ',

    [KEY_KP_STAR] = '*',
    [KEY_KP_MINUS] = '-',
    [KEY_KP_PLUS] = '+',
    [KEY_KP_DOT] = '.',

    [KEY_KP_0] = '0',
    [KEY_KP_1] = '1',
    [KEY_KP_2] = '2',
    [KEY_KP_3] = '3',
    [KEY_KP_4] = '4',
    [KEY_KP_5] = '5',
    [KEY_KP_6] = '6',
    [KEY_KP_7] = '7',
    [KEY_KP_8] = '8',
    [KEY_KP_9] = '9',

    [KEY_KP_ENTER] = '\n',
    [KEY_KP_SLASH] = '/',
};

static inline uint8_t key_pressed(uint8_t scancode) {
    return !(scancode & 0x80);
}

static void keyboard_update_modifiers(keycode_t key, uint8_t pressed) {
    switch (key) {
        case KEY_LEFT_SHIFT:
        case KEY_RIGHT_SHIFT:
            if (pressed) {
                keyboard_modifiers |= KEY_MOD_SHIFT;
            } else {
                keyboard_modifiers &= ~KEY_MOD_SHIFT;
            }
            break;

        case KEY_LEFT_CTRL:
        case KEY_RIGHT_CTRL:
            if (pressed) {
                keyboard_modifiers |= KEY_MOD_CTRL;
            } else {
                keyboard_modifiers &= ~KEY_MOD_CTRL;
            }
            break;

        case KEY_LEFT_ALT:
        case KEY_RIGHT_ALT:
            if (pressed) {
                keyboard_modifiers |= KEY_MOD_ALT;
            } else {
                keyboard_modifiers &= ~KEY_MOD_ALT;
            }
            break;

        case KEY_CAPS_LOCK:
            if (pressed) {
                keyboard_modifiers ^= KEY_MOD_CAPS;
            }
            break;

        default:
            break;
    }
}

static char keycode_to_ascii(keycode_t key, uint32_t modifiers) {
    int shift = modifiers & KEY_MOD_SHIFT;
    int caps = modifiers & KEY_MOD_CAPS;

    if (key >= KEY_A && key <= KEY_Z) {
        int uppercase = shift ^ caps;

        if (uppercase) {
            return key_ascii_shift[key];
        } else {
            return key_ascii_normal[key];
        }
    }

    if (shift) {
        return key_ascii_shift[key];
    }

    return key_ascii_normal[key];
}

static uint32_t keyboard_decode(uint8_t scancode, input_event_t* event) {
    if (!event)
        return 0;
    
    event->pressed = key_pressed(scancode);
    scancode &= 0x7F; 

    event->keycode = scancode_set1_map[scancode];

    keyboard_update_modifiers(event->keycode, event->pressed);
    event->modifiers = keyboard_modifiers;
    
    char c = 0;
    if (event->pressed)
        c = keycode_to_ascii(event->keycode, event->modifiers);

    event->ch = c;

    return 1;
}

void keyboard_callback(regs_t* r) {
    (void)r;
    
    input_event_t event;

    uint8_t scancode = inb(0x60);

    if (keyboard_decode(scancode, &event)) {
        enqueue(&input_buffer, &event);
    }
}

void keyboard_init() {
    input_buffer = queue_create(sizeof(input_event_t));
}
