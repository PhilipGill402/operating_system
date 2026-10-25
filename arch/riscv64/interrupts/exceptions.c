#include "interrupts/exceptions.h"
#include "syscalls.h"
#include <arch/asm/helpers.h>
#include <stdio.h>

void riscv_exception_dispatcher(arch_trapframe_t* tf) {
    if (tf->scause != SCAUSE_ECALL_FROM_U) {
        serial_printf("scause: %l\n", tf->scause);
        serial_printf("sepc: %lx\n", tf->sepc);
        serial_printf("stval: %lx\n", tf->stval);
        serial_printf("sstatus: %lx\n", tf->sstatus);
    } 
    
    uint64_t cause = tf->scause & SCAUSE_CODE_MASK;
    uint8_t fatal = 0;
    switch(cause) {
        case SCAUSE_BREAKPOINT: break;
        case SCAUSE_ILLEGAL_INSTRUCTION: break;
        
        case SCAUSE_INST_ACCESS_FAULT:
        case SCAUSE_LOAD_ACCESS_FAULT:
        case SCAUSE_STORE_ACCESS_FAULT:
        case SCAUSE_INST_PAGE_FAULT:
        case SCAUSE_LOAD_PAGE_FAULT:
        case SCAUSE_STORE_PAGE_FAULT: {
            serial_printf("PAGE FAULT\n");
            for (;;) {
                arch_halt();
            }
    
            break; // will never run, just in case though
        }

        case SCAUSE_ECALL_FROM_U: {
            syscall_handler(tf);
            tf->sepc += 4; // skip past the ecall instruction
            break;
        }
    }
    
    if (fatal) {
        for (;;) {
            arch_halt();
        }
    }
}
