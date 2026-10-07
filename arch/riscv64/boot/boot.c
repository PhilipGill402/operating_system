#include <arch/boot/boot.h>
#include <arch/asm/helpers.h>
#include "io/serial.h"
#include <log.h>
#include <stdio.h>

#define UART0 0x10000000UL

__attribute__((noreturn)) void arch_switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void)) {
    for (;;) {
        arch_halt();
    } 
}

void arch_kernel_early_init(uint32_t mbi_phys) {
    serial_init();
    serial_write("hello world\n");
    //serial_printf("made it");
     
    log_debug("Serial intialized\n");

    for (;;) {
        arch_halt();
    }

    return;
}
