; SAMPLE BOOTLOADER

;org 0x7c00
bits 16
start: jmp boot

msg db "Hello World!", 0ah, 0dh, 0h
boot_drive db 0

boot:
    cli ; no interrupts
    
    ;stack initialization
    xor ax, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    cld ; all that we need to init
    
    mov [boot_drive], dl ; save BIOS boot drive
    
    mov ax, 0x0050

    ; setting up the buffer
    mov es, ax
    xor bx, bx
    
     

    ;read remaining 29 sectors of the floppy disk
    mov ah, 0x02 ; read sectors from disk
    mov al, 2 ; read 2 sector 
    mov ch, 0 ; track 0
    mov cl, 2 ; sector to read
    mov dh, 0 ; head number
    mov dl, [boot_drive] ; drive number
    int 0x13 ; BIOS interrupt
    jc disk_error ; the interrupt failed so jump to error state
    jmp [0x500 + 0x18] ; jump to execute sector
    
    hlt ; halt

disk_error:
    hlt
    jmp disk_error


; Purpose: Move a cursor to a specific location on screen and rember this location

; Parameter:
; bh = y coordinate
; bl = x coordinate

; Return:
; None

_mov_cursor:
    mov ah, 02h ; BIOS: set cursor position 
    mov dh, bh ; row
    mov dl, bl ; col
    xor bh, bh 
    int 10h ; BIOS interrupt
    ret

; Purpose: print a character on screen, at the cursor position previously set by _mov_cursor

; Parameters:
; al = character to print
; bl = text color (ignore)
; cx = number of times the character is repeated (ignore)

; Return:
; None

_put_char:
    mov ah, 0eh ; BIOS: write character and attribute at cursor
    xor bh, bh ; page number : 0
    mov bl, 07h ; text attribute
    int 10h ; BIOS interrupt
    ret

; Purpose: print a string

; Parameters:
; ds:si = zero terminated string

; Return:
; None

_print:
.loop:
    lodsb ; sets al to [ds:si] and increments si
    test al, al ; checks if al is equal to null terminator
    jz .done ; if we found the null terminator then return
    call _put_char ; prints the current character
    jmp .loop ; jump back to start of loop

.done:
    ret






times 510 - ($-$$) db 0
dw 0xAA55
