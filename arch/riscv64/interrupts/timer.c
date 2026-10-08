#include <arch/interrupts/timer.h>
#include "interrupts/timer.h"

#define TIMER_EID 0x54494D45

uint64_t timer_interval = 0;

uint64_t riscv_read_time() {
    uint64_t time;
    asm volatile("csrr %0, time" : "=r"(time));
    return time;
}

int64_t riscv_set_timer(uint64_t deadline) {
    register uint64_t a0 asm("a0") = deadline;
    register uint64_t a6 asm("a6") = 0;
    register uint64_t a7 asm("a7") = TIMER_EID;

    asm volatile ("ecall" : "+r"(a0) : "r"(a6), "r"(a7) : "memory");

    return (int64_t)a0; 
}

int64_t riscv_timer_init(uint64_t timebase_frequency) {
    timer_interval = timebase_frequency / TIMER_FREQUENCY;
    if (timer_interval == 0)
        return -1;
    
    uint64_t time = riscv_read_time();
    int64_t ret = riscv_set_timer(time + timer_interval);
    if (ret < 0)
        return ret;
    
    // enable timer interrupts
    uint64_t mask = 1ULL << 5;
    asm volatile("csrs sie, %0" : : "r"(mask));

    return 0;
}

void arch_timer_ack(void) {
    return;
}
