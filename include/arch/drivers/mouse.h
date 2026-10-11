#ifndef INCLUDE_ARCH_DRIVERS_MOUSE_H_
#define INCLUDE_ARCH_DRIVERS_MOUSE_H_

#include <stdint.h>

#include <log.h>

#include <arch/interrupts/port.h>
#include <arch/asm/helpers.h>

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
} mouse_state_t;

void mouse_init();

#endif
