global isr_wrapper
align 4

extern interrupt_handler

isr_wrapper:
    pushad
    cld
    call interrupt_handler
    popad
    iret
