#include <arch/boot/boot.h>
#include <arch/asm/helpers.h>

__attribute__((noreturn)) void arch_switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void)) {
    for (;;) {
        arch_halt();
    } 

    return;
}

void arch_kernel_early_init(uint32_t mbi_phys) {
    return;
}
