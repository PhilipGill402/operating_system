.section .text
.align 4

.global arch_return_to_user
.type arch_return_to_user, @function

# arch_return_to_user(arch_trapframe_t* tf, uintptr_t kernel_stack_top)
# a0 = tf
# a1 = kernel_stack_top
arch_return_to_user:
.cfi_startproc
    
csrw sscratch, a1   # sscratch = kernel_stack_top

mv t6, a0           # t6 = tf

ld t0, 248(t6)      # t0 = tf->sepc
csrw sepc, t0       # sepc = tf->sepc

ld t0, 256(t6)      # t0 = tf->sstatus
csrw sstatus, t0    # sstatus = tf->sstatus

# Restore general registers
ld ra,   0(t6)
ld gp,  16(t6)
ld tp,  24(t6)

ld t0,  32(t6)
ld t1,  40(t6)
ld t2,  48(t6)

ld s0,  88(t6)
ld s1,  96(t6)
ld s2, 104(t6)
ld s3, 112(t6)
ld s4, 120(t6)
ld s5, 128(t6)
ld s6, 136(t6)
ld s7, 144(t6)
ld s8, 152(t6)
ld s9, 160(t6)
ld s10,168(t6)
ld s11,176(t6)

ld a0, 184(t6)
ld a1, 192(t6)
ld a2, 200(t6)
ld a3, 208(t6)
ld a4, 216(t6)
ld a5, 224(t6)
ld a6, 232(t6)
ld a7, 240(t6)

ld t3,  56(t6)
ld t4,  64(t6)
ld t5,  72(t6)

ld sp, 8(t6)    # sp = tf->sp

ld t6, 80(t6)   # t6 = tf->t6

sret

.cfi_endproc

.size arch_return_to_user, . - arch_return_to_user
