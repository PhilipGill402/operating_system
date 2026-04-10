.global enter_user_mode_from_trapframe
.type enter_user_mode_from_trapframe, @function

enter_user_mode_from_trapframe:
    cli

    # Stack on entry:
    # [esp+4] = tf pointer

    mov 4(%esp), %eax          # eax = tf

    # Load user data selector from tf->ds
    mov 0(%eax), %bx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs

    # Restore general-purpose registers from trapframe
    mov 4(%eax), %edi          # tf->edi
    mov 8(%eax), %esi          # tf->esi
    mov 12(%eax), %ebp         # tf->ebp
    # skip tf->esp at 16
    mov 20(%eax), %ebx         # tf->ebx
    mov 24(%eax), %edx         # tf->edx
    mov 28(%eax), %ecx         # tf->ecx

    # Build iret frame for ring 3
    pushl 60(%eax)             # tf->ss
    pushl 56(%eax)             # tf->useresp
    pushl 52(%eax)             # tf->eflags
    pushl 48(%eax)             # tf->cs
    pushl 44(%eax)             # tf->eip

    # Restore eax last, since we used it as tf pointer
    mov 32(%eax), %eax         # tf->eax

    iret
