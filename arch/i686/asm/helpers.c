#include <arch/asm/helpers.h>

void arch_disable_interrupts(void) {
    __asm__ volatile("cli" ::: "memory");   
}

void arch_enable_interrupts(void) {
    __asm__ volatile("sti" ::: "memory");
}

void arch_halt(void) {
    __asm__ volatile("hlt" ::: "memory");
}
