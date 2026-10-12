.section .text
.align 4

.global riscv_trampoline
.type riscv_trampoline, @function

# riscv_trampoline()
# s0 = void (*entry)(void)
riscv_trampoline:
.cfi_startproc

jalr s0     # call entry function

# TODO: call thread exit function when I create one 

# never return
loop:
    wfi
    j loop

.cfi_endproc

.size riscv_trampoline, . - riscv_trampoline
