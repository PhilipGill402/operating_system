#include <arch/exec/proc.h>
#include "memory/heap.h"
#include <string.h>
#include <log.h>
#include <stddef.h>

extern void riscv_trampoline(void);

arch_trapframe_t* arch_trapframe_init(uintptr_t user_stack_top, uintptr_t entry) {
    arch_trapframe_t* tf = kzmalloc(sizeof(arch_trapframe_t));
    if (!tf)
        return NULL;

    uint64_t aligned_sp = user_stack_top & ~0xFULL; // align stack top to 16 bytes
    tf->sp = aligned_sp;
    tf->sepc = entry;

    uint64_t sstatus = 0;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1ULL << 1);    // SIE = 0 
    sstatus &= ~(1ULL << 8);    // SPP = 0
    sstatus |= 1ULL << 5;       // SPIE = 1
    tf->sstatus = sstatus;

    return tf;
}

void arch_trapframe_copy(arch_trapframe_t* dst, arch_trapframe_t* src) {
    memcpy(dst, src, sizeof(arch_trapframe_t));
}

void arch_trapframe_reset(arch_trapframe_t* tf, uintptr_t user_stack_top, uintptr_t entry) {
    uint64_t aligned_sp = user_stack_top & ~0xFULL; // align stack top to 16 bytes
    tf->sp = aligned_sp;
    tf->sepc = entry;

    uint64_t sstatus = 0;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1ULL << 1);    // SIE = 0 
    sstatus &= ~(1ULL << 8);    // SPP = 0
    sstatus |= 1ULL << 5;       // SPIE = 1
    tf->sstatus = sstatus;
}

void arch_trapframe_destroy(arch_trapframe_t* tf) {
    kfree(tf); 
}

void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret) {
    tf->a0 = ret;
}

uint32_t arch_trapframe_get_arg(arch_trapframe_t* tf, uint32_t idx) {
    switch(idx) {
        case 1: return tf->a0;
        case 2: return tf->a1;
        case 3: return tf->a2;
        case 4: return tf->a3;
        case 5: return tf->a4;
        case 6: return tf->a5;
        case 7: return tf->a6;
        case 8: return tf->a7;
        default: log_error("tried to access unsupported argument number %u\n", idx); return 0;
    }
}

uint8_t arch_trapframe_from_user(arch_trapframe_t* tf) {
    return !(tf->sstatus & (1ULL << 8));    // check SPP
}

arch_context_t* arch_context_init(uintptr_t kernel_stack_top, void (*entry)(void)) {
    arch_context_t* ctx = kzmalloc(sizeof(arch_context_t));
    if (!ctx)
        return NULL;
 
    uint64_t aligned_sp = kernel_stack_top & ~0xFULL; // align stack top to 16 bytes

    ctx->kernel_ctx.ra = (uintptr_t)riscv_trampoline;
    ctx->kernel_ctx.s0 = (uintptr_t)entry;
    ctx->kernel_ctx.sp = aligned_sp;
    ctx->kernel_sp = aligned_sp;
    asm volatile("csrr %0, satp" : "=r"(ctx->satp));
    
    return ctx;
}

void arch_context_destroy(arch_context_t* ctx) {
    kfree(ctx);
}

void arch_trapframe_debug(const arch_trapframe_t* tf) {
    if (!tf) {
        log_error("trapframe: NULL\n");
        return;
    }

    log_debug("ra:      %l\n", tf->ra);
    log_debug("sp:      %l\n", tf->sp);
    log_debug("gp:      %l\n", tf->gp);
    log_debug("tp:      %l\n", tf->tp);

    log_debug("t0:      %l\n", tf->t0);
    log_debug("t1:      %l\n", tf->t1);
    log_debug("t2:      %l\n", tf->t2);
    log_debug("t3:      %l\n", tf->t3);
    log_debug("t4:      %l\n", tf->t4);
    log_debug("t5:      %l\n", tf->t5);
    log_debug("t6:      %l\n", tf->t6);

    log_debug("s0:      %l\n", tf->s0);
    log_debug("s1:      %l\n", tf->s1);
    log_debug("s2:      %l\n", tf->s2);
    log_debug("s3:      %l\n", tf->s3);
    log_debug("s4:      %l\n", tf->s4);
    log_debug("s5:      %l\n", tf->s5);
    log_debug("s6:      %l\n", tf->s6);
    log_debug("s7:      %l\n", tf->s7);
    log_debug("s8:      %l\n", tf->s8);
    log_debug("s9:      %l\n", tf->s9);
    log_debug("s10:     %l\n", tf->s10);
    log_debug("s11:     %l\n", tf->s11);

    log_debug("a0:      %l\n", tf->a0);
    log_debug("a1:      %l\n", tf->a1);
    log_debug("a2:      %l\n", tf->a2);
    log_debug("a3:      %l\n", tf->a3);
    log_debug("a4:      %l\n", tf->a4);
    log_debug("a5:      %l\n", tf->a5);
    log_debug("a6:      %l\n", tf->a6);
    log_debug("a7:      %l\n", tf->a7);

    log_debug("sepc:    %l\n", tf->sepc);
    log_debug("sstatus: %l\n", tf->sstatus);
    log_debug("scause:  %l\n", tf->scause);
    log_debug("stval:   %l\n", tf->stval);
}
