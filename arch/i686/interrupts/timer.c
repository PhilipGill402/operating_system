#include <arch/interrupts/timer.h>
#include "include/pic.h"

void arch_timer_ack(void) {
    pic_send_eoi(IRQ_TIMER);
}
