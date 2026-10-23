.section .boot.bss, "aw", @nobits
.align 3
trap_scause:
    .dword 0
trap_sepc:
    .dword 0
trap_stval:
    .dword 0

.align 12
boot_root_pt:
    .skip 4096

.section .init, "ax", @progbits

.equ PTE_V,  (1 << 0)
.equ PTE_R,  (1 << 1)
.equ PTE_W,  (1 << 2)
.equ PTE_X,  (1 << 3)
.equ PTE_A,  (1 << 6)
.equ PTE_D,  (1 << 7)

.equ BOOT_PTE_FLAGS, (PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D)

.option norvc

.global start

start:
    # Don't allow interrupts during early boot
    csrw sie, zero
    csrci sstatus, 2
    csrw sscratch, zero

    # Set stack
    la sp, boot_stack_top

    # Clear physical backing of kernel BSS
    la t5, bss_phys_start
    la t6, bss_phys_end

bss_clear:
    bgeu t5, t6, bss_done
    sd zero, 0(t5)
    addi t5, t5, 8
    j bss_clear

bss_done:
    call setup_boot_paging

    la t0, higher_half_entry_ptr
    ld t0, 0(t0)
    jr t0

setup_boot_paging:
    # clear boot root page table 
    la t0, boot_root_pt
    li t1, 512

1:
    sd zero, 0(t0)
    addi t0, t0, 8
    addi t1, t1, -1
    bnez t1, 1b
    
    la t0, boot_root_pt
    
    li t2, BOOT_PTE_FLAGS
    sd t2, 0(t0)

    # create 1 GiB leaf PTE
    li t1, 0x80000000
    srli t1, t1, 12
    slli t1, t1, 10
    ori t1, t1, BOOT_PTE_FLAGS
    
    # identity map for low half
    sd t1, 16(t0)
    
    # map higher half
    li t2, 2048
    add t2, t0, t2
    sd t1, 0(t2)
    
    # set satp = Sv39 | root_ppn
    la t0, boot_root_pt
    srli t0, t0, 12

    li t1, 8
    slli t1, t1, 60
    or t0, t0, t1
    csrw satp, t0
    sfence.vma zero, zero

    ret

.align 3
higher_half_entry_ptr:
    .dword higher_half_entry


.section .text, "ax", @progbigs
.global higher_half_entry
.type higher_half_entry, @function

higher_half_entry:
.option push
.option norelax
    la gp, __global_pointer$
.option pop

    la sp, stack_top

    call riscv_kernel_entry

1:
    j 1b

