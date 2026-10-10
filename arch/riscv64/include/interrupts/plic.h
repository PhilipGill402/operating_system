#ifndef ARCH_RISCV_INCLUDE_INTERRUPTS_PLIC_H_
#define ARCH_RISCV_INCLUDE_INTERRUPTS_PLIC_H_

#define PLIC_BASE              0x0C000000UL

#define PLIC_PRIORITY_BASE     (PLIC_BASE + 0x000000)
#define PLIC_PENDING_BASE      (PLIC_BASE + 0x001000)

#define PLIC_ENABLE_BASE       (PLIC_BASE + 0x002000)
#define PLIC_ENABLE_STRIDE     0x80

#define PLIC_CONTEXT_BASE      (PLIC_BASE + 0x200000)
#define PLIC_CONTEXT_STRIDE    0x1000

#define PLIC_PRIORITY(irq) \
    (PLIC_BASE + ((irq) * 4))

#define PLIC_SENABLE(hart) \
    (PLIC_BASE + 0x2080 + ((hart) * 0x100))

#define PLIC_STHRESHOLD(hart) \
    (PLIC_BASE + 0x201000 + ((hart) * 0x2000))

#define PLIC_SCLAIM(hart) \
    (PLIC_BASE + 0x201004 + ((hart) * 0x2000))

#define PLIC_UART0_IRQ         10

void riscv_plic_init();

#endif
