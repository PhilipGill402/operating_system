ENTRY(start)

SECTIONS
{
    . = 0x80200000;

    .text : ALIGN(4K)
    {
        KEEP(*(.init))
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K)
    {
        PROVIDE(bss_start = .);

        *(.bss)
        *(.bss.*)
        *(COMMON)

        PROVIDE(bss_end = .);
    }

    . = ALIGN(16);

    PROVIDE(stack_bottom = .);
    . += 16K;
    PROVIDE(stack_top = .);

    PROVIDE(kernel_end = .);
}
