
ENTRY(_start)

SECTIONS
{
  . = 0x1000;          /* wherever your bootloader loads the kernel */
  .text : 
  { 
      KEEP(*(.text._start))   /* force stub first */
      *(.text*)
  }
  .rodata : 
  { 
      *(.rodata*) 
  }
  .data : 
  { 
      *(.data*) 
  }
  .bss : 
  { 
      *(COMMON) 
      *(.bss*) 
  }
}
