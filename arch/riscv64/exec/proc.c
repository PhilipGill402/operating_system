#include <arch/exec/proc.h>
#include <stddef.h>

arch_trapframe_t* arch_trapframe_init(uintptr_t user_stack_top, uintptr_t entry) {
    return NULL;
}

void arch_trapframe_copy(arch_trapframe_t* dst, arch_trapframe_t* src) {
    return;
}

void arch_trapframe_reset(arch_trapframe_t* tf, uintptr_t user_stack_top, uintptr_t entry) {
    return;
}

void arch_trapframe_destroy(arch_trapframe_t* tf) {
    return;
}

void arch_trapframe_set_ret(arch_trapframe_t* tf, uint32_t ret) {
    return;
}

uint32_t arch_trapframe_get_arg(arch_trapframe_t* tf, uint32_t idx) {
    return 0;
}

uint8_t arch_trapframe_from_user(arch_trapframe_t* tf) {
    return 0;
}

arch_context_t* arch_context_init(uintptr_t kernel_stack_top, void (*entry)(void)) {
    return NULL;
}

void arch_context_destroy(arch_context_t* ctx) {
    return;
}

void arch_set_kernel_stack(uint32_t stack_top) {
    return;
}

void arch_trapframe_debug(const arch_trapframe_t* tf) {
    return;
}
