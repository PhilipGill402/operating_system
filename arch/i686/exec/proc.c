#include <arch/proc.h>

#include <memory/heap.h>

arch_trapframe_t* arch_trapframe_init(uint32_t* kernel_stack_top, uintptr_t user_stack_top, uintptr_t entry) {
    arch_trapframe_t* tf = (arch_trapframe_t*)(kernel_stack_top - sizeof(arch_trapframe_t));
    memset(tf, 0, sizeof(tf));

    tf->eip = entry;
    tf->cs = USER_CS_RING3;
    tf->eflags = 0x202;
    tf->useresp = user_stack_top;
    tf->ss = USER_DS_RING3;
    tf->ds = USER_DS_RING3;

    arch_trapframe_t* proc_tf = kmalloc(sizeof(arch_trapframe_t));
    if (!proc_tf)
        return NULL;

    memcpy(proc_tf, tf, sizeof(arch_trapframe_t));

    return proc_tf;

    *kernel_stack_top = (uint32_t)tf;

    return tf;
}

void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret) {
    tf->eax = ret;
}

void arch_context_init(arch_context_t* ctx, uintptr_t kernel_stack_top, void (*entry)(void)) {
    if (!ctx || !kernel_stack_top || !entry)
        return;

    uint32_t* sp = (uint32_t*)kernel_stack_top;
    
    /*
     * restores:
     * edi
     * esi
     * ebx
     * ebp
     */

    *--sp = (uint32_t)entry;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;

    ctx->esp = (uint32_t)sp;
}
