.section .bss
.align 3
trap_scause:
    .dword 0
trap_sepc:
    .dword 0
trap_stval:
    .dword 0

.section .init

.option norvc

.type start, @function
.global start

start:
    .cfi_startproc

    # Don't allow interrupts during early boot
    csrw sie, zero
    csrci sstatus, 2

    # Give unexpected traps somewhere known to go
    la t0, early_trap
    csrw stvec, t0

.option push
.option norelax
    la gp, __global_pointer$
.option pop

    # Disable paging
    csrw satp, zero
    sfence.vma zero, zero

    # Set stack
    la sp, stack_top

    # Clear BSS
    la t5, bss_start
    la t6, bss_end

bss_clear:
    bgeu t5, t6, bss_done
    sd zero, 0(t5)
    addi t5, t5, 8
    j bss_clear

bss_done:
    tail kernel_main


early_trap:
    csrr t0, scause
    la t1, trap_scause
    sd t0, 0(t1)

    csrr t0, sepc
    la t1, trap_sepc
    sd t0, 0(t1)

    csrr t0, stval
    la t1, trap_stval
    sd t0, 0(t1)

    li t0, 0x10000000
    li t1, 'T'
    sb t1, 0(t0)

1:
    j 1b
    .cfi_endproc
