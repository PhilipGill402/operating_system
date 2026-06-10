#ifndef INCLUDE_INTERRUPTS_MOUSE_H_
#define INCLUDE_INTERRUPTS_MOUSE_H_

#include <stdint.h>

#include <log.h>

#include "io/framebuffer/framebuffer.h"
#include "interrupts/irq.h"
#include "interrupts/keyboard.h"
#include "interrupts/port.h"

#define PS2_DATA_PORT      0x60
#define PS2_STATUS_PORT    0x64
#define PS2_COMMAND_PORT   0x64

#define PS2_STATUS_OUTPUT_FULL 0x01
#define PS2_STATUS_INPUT_FULL  0x02

#define PS2_CMD_READ_CONFIG    0x20
#define PS2_CMD_WRITE_CONFIG   0x60
#define PS2_CMD_ENABLE_AUX     0xA8
#define PS2_CMD_WRITE_MOUSE    0xD4

#define MOUSE_CMD_RESET        0xFF
#define MOUSE_CMD_ENABLE       0xF4
#define MOUSE_CMD_SET_DEFAULTS 0xF6

#define MOUSE_ACK              0xFA

#define MOUSE_WIDTH 10
#define MOUSE_HEIGHT 10

typedef struct {
    uint32_t x;
    uint32_t y;

    uint32_t prev_x;
    uint32_t prev_y;

    uint32_t dx;
    uint32_t dy;

    uint8_t left_clicked;
    uint8_t right_clicked;
    uint8_t middle_clicked;

    uint8_t dirty;
} mouse_state_t;

void mouse_init();
void mouse_draw();

#endif // !INCLUDE_INTERRUPTS_MOUSE_H_
