#include <arch/interrupts/timer.h>
#include "interrupts/pic.h"

void arch_timer_ack(void) {
    pic_send_eoi(IRQ_TIMER);
}
