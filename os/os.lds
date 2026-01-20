ENTRY(main);

PHDRS{
headers PT_PHDR FILEHDR PHDRS AT(0x500);
code PT_LOAD FILEHDR PHDRS FLAGS(0x1 | 0x2 | 0x4);
}
SECTIONS /* Command */
{
. = 0x0;
. = + SIZEOF_HEADERS;
.text : { *(.text) } :code
/DISCARD/ : { *(.eh_frame) }
}
