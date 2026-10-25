#include <arch/boot/boot.h>
#include <arch/asm/helpers.h>
#include "kernel.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "boot/dtb.h"
#include "drivers/uart.h"
#include "interrupts/irq.h"
#include <log.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((noreturn)) void arch_switch_to_new_kernel_stack(uint32_t new_stack_top, void (*next)(void)) {
    for (;;) {
        arch_halt();
    } 
}

void riscv_kernel_entry(uint64_t hart_id, uintptr_t dtb_phys) {
    kernel_boot_info_t info;
    
    dtb_tree_t* dtb = (dtb_tree_t*)dtb_phys;
    if (!dtb_init(dtb))
        panic("invalid DTB passed\n");

    if (!dtb_parse_struct(dtb, &info))
        panic("invalid DTB passed\n");


    pmm_init(&info);

    if (!riscv_paging_init())
        panic("failed to initialize paging\n");
    
    riscv_irq_init();
    uart_init();
    log_debug("uart initialized\n");

    kernel_main(&info);

    return;
}
