#include "interrupts/exceptions.h"
#include <arch/asm/helpers.h>
#include <stdio.h>

void riscv_exception_dispatcher(arch_trapframe_t* tf) {
    serial_printf("scause: %l\n", tf->scause);
    serial_printf("sepc: %l\n", tf->sepc);
    serial_printf("stval: %l\n", tf->stval);
    serial_printf("sstatus: %l\n", tf->sstatus);
    
    uint64_t cause = tf->scause & SCAUSE_CODE_MASK;
    uint8_t fatal = 0;
    switch(cause) {
        case SCAUSE_BREAKPOINT: break;
        case SCAUSE_ILLEGAL_INSTRUCTION: break;
        case SCAUSE_LOAD_ACCESS_FAULT: break;
        case SCAUSE_STORE_ACCESS_FAULT: break;
    }
    
    if (fatal) {
        for (;;) {
            arch_halt();
        }
    }
}
