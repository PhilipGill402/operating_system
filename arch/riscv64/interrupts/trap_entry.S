.section .text
.align 4

.extern riscv_trap_handler
.global riscv_trap_entry
.type riscv_trap_entry, @function

riscv_trap_entry:
    .cfi_startproc
# TODO: switch to kernel stack

# save registers
    addi sp, sp, -288

    sd ra, 0(sp)
    sd t0, 32(sp)

# save original stack pointer
    addi t0, sp, 288
    sd t0, 8(sp)

    sd gp, 16(sp)
    sd tp, 24(sp)

    sd t1, 40(sp)
    sd t2, 48(sp)
    sd t3, 56(sp)
    sd t4, 64(sp)
    sd t5, 72(sp)
    sd t6, 80(sp)

    sd s0, 88(sp)
    sd s1, 96(sp)
    sd s2, 104(sp)
    sd s3, 112(sp)
    sd s4, 120(sp)
    sd s5, 128(sp)
    sd s6, 136(sp)
    sd s7, 144(sp)
    sd s8, 152(sp)
    sd s9, 160(sp)
    sd s10, 168(sp)
    sd s11, 176(sp)

    sd a0, 184(sp)
    sd a1, 192(sp)
    sd a2, 200(sp)
    sd a3, 208(sp)
    sd a4, 216(sp)
    sd a5, 224(sp)
    sd a6, 232(sp)
    sd a7, 240(sp)

# save CSRs
    csrr t0, sepc
    sd t0, 248(sp)

    csrr t0, sstatus
    sd t0, 256(sp)

    csrr t0, scause
    sd t0, 264(sp)

    csrr t0, stval
    sd t0, 272(sp)

# 280(sp) is padding
    
    
# call C handler
    mv a0, sp
    call riscv_trap_handler

# restore CSRs that control return
    ld t0, 248(sp)
    csrw sepc, t0

    ld t0, 256(sp)
    csrw sstatus, t0

# restore general-purpose registers
    ld ra, 0(sp)

    ld gp, 16(sp)
    ld tp, 24(sp)

    ld t1, 40(sp)
    ld t2, 48(sp)
    ld t3, 56(sp)
    ld t4, 64(sp)
    ld t5, 72(sp)
    ld t6, 80(sp)

    ld s0, 88(sp)
    ld s1, 96(sp)
    ld s2, 104(sp)
    ld s3, 112(sp)
    ld s4, 120(sp)
    ld s5, 128(sp)
    ld s6, 136(sp)
    ld s7, 144(sp)
    ld s8, 152(sp)
    ld s9, 160(sp)
    ld s10, 168(sp)
    ld s11, 176(sp)

    ld a0, 184(sp)
    ld a1, 192(sp)
    ld a2, 200(sp)
    ld a3, 208(sp)
    ld a4, 216(sp)
    ld a5, 224(sp)
    ld a6, 232(sp)
    ld a7, 240(sp)

# restore t0 last, since we've been using it as scratch
    ld t0, 32(sp)

# restore original stack pointer last
    ld sp, 8(sp)

    sret

    .cfi_endproc

.size riscv_trap_entry, . - riscv_trap_entry
