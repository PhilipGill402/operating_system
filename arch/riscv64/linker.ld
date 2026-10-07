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

        PROVIDE(__global_pointer$ = . + 0x800);

        *(.sdata)
        *(.sdata.*)
    }

    .bss : ALIGN(4K)
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

    . = ALIGN(16);

    stack_bottom = .;
    . += 16K;
    stack_top = .;

    kernel_end = .;
}
