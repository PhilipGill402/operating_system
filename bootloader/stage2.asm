org 0x0000
bits 16

start:
    mov ax, cs
    mov ds, ax

    mov si, msg
    call print

hang:
    jmp hang

_put_char:
    mov ah, 0eh ; BIOS: write character and attribute at cursor
    xor bh, bh ; page number : 0
    mov bl, 07h ; text attribute
    int 10h ; BIOS interrupt
    ret

_print:
.loop:
    lodsb ; sets al to [ds:si] and increments si
    test al, al ; checks if al is equal to null terminator
    jz .done ; if we found the null terminator then return
    call _put_char ; prints the current character
    jmp .loop ; jump back to start of loop

.done:
    ret
