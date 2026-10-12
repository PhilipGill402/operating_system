.section .text
.align 4

.global riscv_context_switch
.type riscv_context_switch, @function

# riscv_context_switch(arch_context_t* old, arch_context_t* new)
# a0 = old
# a1 = new
riscv_context_switch:
.cfi_startproc
    
# save ra, sp, and s0-s11 into old
sd ra, 8(a0)        # old->kernel_ctx.ra = ra
sd sp, 16(a0)       # old->kernel_ctx.sp = sp
sd s0, 24(a0)       # old->kernel_ctx.s0 = s0
sd s1, 32(a0)       # old->kernel_ctx.s1 = s1
sd s2, 40(a0)       # old->kernel_ctx.s2 = s2
sd s3, 48(a0)       # old->kernel_ctx.s3 = s3
sd s4, 56(a0)       # old->kernel_ctx.s4 = s4
sd s5, 64(a0)       # old->kernel_ctx.s5 = s5
sd s6, 72(a0)       # old->kernel_ctx.s6 = s6
sd s7, 80(a0)       # old->kernel_ctx.s7 = s7
sd s8, 88(a0)       # old->kernel_ctx.s8 = s8
sd s9, 96(a0)       # old->kernel_ctx.s9 = s9
sd s10, 104(a0)     # old->kernel_ctx.s10 = s10
sd s11, 112(a0)     # old->kernel_ctx.s11 = s11

# load ra, sp, and s0-s11 from new
ld s0, 24(a1)       # s0 = new->kernel_ctx.s0
ld s1, 32(a1)       # s1 = new->kernel_ctx.s1
ld s2, 40(a1)       # s2 = new->kernel_ctx.s2
ld s3, 48(a1)       # s3 = new->kernel_ctx.s3
ld s4, 56(a1)       # s4 = new->kernel_ctx.s4
ld s5, 64(a1)       # s5 = new->kernel_ctx.s5
ld s6, 72(a1)       # s6 = new->kernel_ctx.s6
ld s7, 80(a1)       # s7 = new->kernel_ctx.s7
ld s8, 88(a1)       # s8 = new->kernel_ctx.s8
ld s9, 96(a1)       # s9 = new->kernel_ctx.s9
ld s10, 104(a1)     # s10 = new->kernel_ctx.s10
ld s11, 112(a1)     # s11 = new->kernel_ctx.s11
ld ra, 8(a1)        # ra = new->kernel_ctx.ra
ld sp, 16(a1)       # sp = new->kernel_ctx.sp

ret

.cfi_endproc

.size riscv_context_switch, . - riscv_context_switch
