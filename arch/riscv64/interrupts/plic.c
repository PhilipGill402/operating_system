#include "interrupts/plic.h"
#include <arch/asm/helpers.h>
#include <arch/interrupts/port.h>

static void riscv_uart_enable() {
    arch_write_u32(PLIC_PRIORITY(PLIC_UART0_IRQ), 1); // set UART priority to 1
    
    // enable UART0 IRQ
    uint32_t senable = arch_read_u32(PLIC_SENABLE(0));
    senable |= 1U << PLIC_UART0_IRQ;
    arch_write_u32(PLIC_SENABLE(0), senable);
    
    arch_write_u32(PLIC_STHRESHOLD(0), 0); // set UART threshold to 0
}

void riscv_plic_init() {
    riscv_uart_enable(); 

    // enable SEIE
    uint64_t mask = 1ULL << 9;
    asm volatile("csrs sie, %0" : : "r"(mask));
}
