#include "timer.h"

volatile uint32_t ticks = 0;

void timer_callback(regs_t* r) {
    (void)r;
    ticks++;
}

uint32_t timer_get_ticks() {
    return ticks;
}
