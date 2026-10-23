ENTRY(start)

PAGE_SIZE = 0x1000;

/*
 * QEMU virt/OpenSBI loads the kernel starting here.
 */
KERNEL_PHYS_BASE = 0x80200000;

/*
 * Bootstrap Sv39 mapping:
 *
 *   VA 0xFFFFFFC000000000 -> PA 0x80000000
 *
 * Therefore the constant VA-PA offset is:
 */
HIGHER_HALF_BASE = 0xFFFFFFC000000000;
RAM_PHYS_BASE    = 0x80000000;
KERNEL_OFFSET    = HIGHER_HALF_BASE - RAM_PHYS_BASE;

/*
 * PA 0x80200000 therefore appears at:
 *
 *   VA 0xFFFFFFC000200000
 */
KERNEL_VIRT_BASE = KERNEL_PHYS_BASE + KERNEL_OFFSET;


SECTIONS
{
    /*
     * ================================================================
     * LOW BOOTSTRAP
     *
     * Paging is OFF here.
     * VMA == physical address.
     * ================================================================
     */

    . = KERNEL_PHYS_BASE;

    boot_start = .;


    /*
     * Must contain `start`, setup_boot_paging, early_trap, etc.
     */
    .init ALIGN(PAGE_SIZE) :
    {
        KEEP(*(.init))
        KEEP(*(.init.*))
    }


    /*
     * Temporary stack used before jumping into the higher half.
     */
    .boot_stack ALIGN(16) (NOLOAD) :
    {
        boot_stack_bottom = .;

        . += 16K;

        boot_stack_top = .;
    }


    /*
     * Early data that must be accessible before paging.
     *
     * This includes:
     *   trap_scause
     *   trap_sepc
     *   trap_stval
     *   boot_root_pt
     */
    .boot.bss ALIGN(PAGE_SIZE) (NOLOAD) :
    {
        boot_bss_start = .;

        *(.boot.bss)
        *(.boot.bss.*)

        . = ALIGN(PAGE_SIZE);

        boot_bss_end = .;
    }


    . = ALIGN(PAGE_SIZE);

    boot_end = .;

    /*
     * First physical address occupied by the normal kernel.
     */
    kernel_phys_start = .;


    /*
     * ================================================================
     * HIGHER-HALF KERNEL
     * ================================================================
     *
     * Move the VMA upward while keeping LMAs in physical RAM.
     */

    . = kernel_phys_start + KERNEL_OFFSET;

    kernel_start = .;


    /*
     * ------------------------------------------------
     * TEXT
     * ------------------------------------------------
     */

    .text ALIGN(PAGE_SIZE) : AT(kernel_phys_start)
    {
        *(.text)
        *(.text.*)
    }

    text_phys_end = LOADADDR(.text) + SIZEOF(.text);


    /*
     * ------------------------------------------------
     * READ-ONLY DATA
     * ------------------------------------------------
     */

    .rodata ALIGN(PAGE_SIZE) :
        AT(ALIGN(text_phys_end, PAGE_SIZE))
    {
        *(.rodata)
        *(.rodata.*)

        *(.srodata)
        *(.srodata.*)

        *(.eh_frame)
        *(.eh_frame.*)
    }

    rodata_phys_end = LOADADDR(.rodata) + SIZEOF(.rodata);


    /*
     * ------------------------------------------------
     * WRITABLE DATA
     * ------------------------------------------------
     */

    .data ALIGN(PAGE_SIZE) :
        AT(ALIGN(rodata_phys_end, PAGE_SIZE))
    {
        *(.data)
        *(.data.*)

        PROVIDE(__global_pointer$ = . + 0x800);

        *(.sdata)
        *(.sdata.*)
    }

    data_phys_end = LOADADDR(.data) + SIZEOF(.data);


    /*
     * ------------------------------------------------
     * BSS
     * ------------------------------------------------
     */

    .bss ALIGN(PAGE_SIZE) (NOLOAD) :
        AT(ALIGN(data_phys_end, PAGE_SIZE))
    {
        bss_start = .;

        *(.sbss)
        *(.sbss.*)

        *(.bss)
        *(.bss.*)

        *(COMMON)

        . = ALIGN(8);

        bss_end = .;
    }

    /*
     * Low physical addresses used by start() before paging.
     */
    bss_phys_start = LOADADDR(.bss);
    bss_phys_end   = LOADADDR(.bss) + SIZEOF(.bss);


    /*
     * ------------------------------------------------
     * PERMANENT HIGHER-HALF KERNEL STACK
     * ------------------------------------------------
     */

    .kernel_stack ALIGN(PAGE_SIZE) (NOLOAD) :
        AT(ALIGN(bss_phys_end, PAGE_SIZE))
    {
        stack_bottom = .;

        . += 16K;

        stack_top = .;
    }


    . = ALIGN(PAGE_SIZE);

    kernel_end = .;

    kernel_phys_end =
        LOADADDR(.kernel_stack) + SIZEOF(.kernel_stack);
}
