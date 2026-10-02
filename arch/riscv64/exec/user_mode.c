#include <arch/exec/user_mode.h>
#include <arch/asm/helpers.h>

__attribute__((noreturn)) void arch_return_to_user(const arch_trapframe_t* tf) {
    for (;;) {
        arch_halt();
    }

    return;
}

void arch_context_switch(arch_context_t* old_ctx, arch_context_t* new_ctx) {
    return;
}
