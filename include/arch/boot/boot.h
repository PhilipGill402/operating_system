#ifndef ARCH_I686_BOOT_H_
#define ARCH_I686_BOOT_H_

#include <stdint.h>

#include "multiboot.h"

__attribute__((noreturn)) void arch_switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void));
void arch_kernel_early_init(uint32_t mbi_phys);

// declared in main kernel file
extern multiboot_info_t* mbi;

#endif // !ARCH_I686_BOOT_H_
