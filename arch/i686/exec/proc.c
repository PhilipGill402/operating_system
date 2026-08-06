#include <arch/exec/proc.h>
#include <arch/cpu/tss.h>
#include <exec/trapframe.h>
#include <exec/context.h>
#include <string.h>

#include "memory/heap.h"

#include <log.h>

arch_trapframe_t* arch_trapframe_init(uintptr_t user_stack_top, uintptr_t entry) {
    arch_trapframe_t* tf = kzmalloc(sizeof(*tf));
    if (!tf)
        return NULL;

    tf->eip = (uint32_t)entry;
    tf->cs = USER_CS_RING3;
    tf->eflags = 0x202;
    tf->useresp = (uint32_t)user_stack_top;
    tf->ss = USER_DS_RING3;
    tf->ds = USER_DS_RING3;

    return tf;
}

void arch_trapframe_reset(arch_trapframe_t* tf, uintptr_t user_stack_top, uintptr_t entry) {
    memset(tf, 0, sizeof(*tf));
    
    tf->eip = (uint32_t)entry;
    tf->cs = USER_CS_RING3;
    tf->eflags = 0x202;
    tf->useresp = (uint32_t)user_stack_top;
    tf->ss = USER_DS_RING3;
    tf->ds = USER_DS_RING3;
}

void arch_trapframe_copy(arch_trapframe_t* dst, arch_trapframe_t* src) {
    if (!dst || !src)
        return;

    memcpy(dst, src, sizeof(*src));
}

void arch_trapframe_destroy(arch_trapframe_t* tf) {
    kfree(tf);
}

void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret) {
    tf->eax = ret;
}

uint32_t arch_trapframe_get_arg(arch_trapframe_t* tf, uint32_t idx) {
    switch (idx) {
        case 1:
            return tf->eax;
        case 2:
            return tf->ebx;
        case 3:
            return tf->ecx;
        case 4:
            return tf->edx;
        case 5:
            return tf->esi;
        case 6:
            return tf->edi;
        case 7:
            return tf->ebp;
        default:
            return 0;
    }
}

arch_context_t* arch_context_init(uintptr_t kernel_stack_top, void (*entry)(void)) {
    arch_context_t* ctx = kmalloc(sizeof(*ctx)); 

    if (!ctx || !kernel_stack_top || !entry)
        return NULL;

    uint32_t* sp = (uint32_t*)kernel_stack_top;
    
    /*
     * restores:
     * edi
     * esi
     * ebx
     * ebp
     *
     */

    *--sp = (uint32_t)entry;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;

    ctx->esp = (uint32_t)sp;

    return ctx;
}

void arch_context_destroy(arch_context_t* ctx) {
    kfree(ctx);
}

uint8_t arch_trapframe_from_user(arch_trapframe_t* tf) {
    return (tf->cs & 0x3) == 0x3;
}

void arch_set_kernel_stack(uint32_t stack_top) {
    tss_set_kernel_stack(stack_top);
}

void arch_trapframe_debug(const arch_trapframe_t *tf) {
    if (!tf) {
        log_error("trapframe: NULL\n");
        return;
    }

    log_debug(
        "TF=%x eip=%x cs=%x eflags=%x "
        "useresp=%x ss=%x ds=%x\n",
        (uint32_t)(uintptr_t)tf,
        tf->eip,
        tf->cs,
        tf->eflags,
        tf->useresp,
        tf->ss,
        tf->ds
    );

    log_debug(
        "eax=%x ebx=%x ecx=%x edx=%x "
        "esi=%x edi=%x ebp=%x esp=%x\n",
        tf->eax,
        tf->ebx,
        tf->ecx,
        tf->edx,
        tf->esi,
        tf->edi,
        tf->ebp,
        tf->esp
    );

    log_debug(
        "int_no=%x err_code=%x\n",
        tf->int_no,
        tf->err_code
    );
}
