#include <arch/asm/helpers.h>

void arch_disable_interrupts(void) {
    __asm__ volatile("csrci sstatus, 2" ::: "memory");
}

void arch_enable_interrupts(void) {
    __asm__ volatile("csrsi sstatus, 2" ::: "memory");
}

void arch_halt(void) {
    __asm__ volatile("wfi");
}

void arch_cpu_relax(void) {
    __asm__ volatile("nop");
}
