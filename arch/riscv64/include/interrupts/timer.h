#ifndef ARCH_RISCV64_INCLUDE_INTERRUPTS_TIMER_H_
#define ARCH_RISCV64_INCLUDE_INTERRUPTS_TIMER_H_

#include <stdint.h>

#define TIMEBASE_FREQUENCY 10000000ULL

uint64_t riscv_read_time();
int64_t riscv_set_timer(uint64_t deadline);
int64_t riscv_timer_init(uint64_t timebase_frequency);

extern uint64_t timer_interval;

#endif
