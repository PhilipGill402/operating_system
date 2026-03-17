ENTRY(_start)

SECTIONS
{
    . = 2M;

    .text BLOCK(4k) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }
    
    .rodata BLOCK(4k) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4k) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4k) : ALIGN(4K)
    {
        *(.bss)
    }


}
