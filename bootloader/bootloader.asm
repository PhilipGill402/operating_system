; bootloader.asm
section .boot
bits 16
global boot

boot:
    ; Make data-label addressing correct (BIOS does NOT guarantee DS=CS)
    push cs
    pop ds

    ; Simple stack
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Text mode
    mov ax, 0x0003
    int 0x10

    mov [boot_drive], dl

    ; Read kernel into 0000:1000
    xor ax, ax
    mov es, ax
    mov bx, 0x1000

    mov ah, 0x02        ; read sectors
    mov al, 10          ; <-- update if kernel grows
    mov ch, 0
    mov dh, 0
    mov cl, 2           ; sector 2
    mov dl, [boot_drive]
    int 0x13
    jc disk_fail

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEL:pm_entry

disk_fail:
    mov ax, 0x0003
    int 0x10
    mov si, failmsg
.print:
    lodsb
    test al, al
    jz $
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp .print

failmsg db "DISK READ FAIL",0

; ---------------- Protected Mode ----------------
bits 32
pm_entry:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov dword [0xB8000], 0x0F4B0F4F   ; 'O''K'
    jmp CODE_SEL:0x1000               ; jump to kernel _start

; ---------------- GDT ----------------
bits 16
gdt_start:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEL equ gdt_code - gdt_start
DATA_SEL equ gdt_data - gdt_start

boot_drive db 0

times 510 - ($ - $$) db 0
dw 0xAA55

