.intel_syntax noprefix

.global arch_context_switch
.type arch_context_switch, @function

arch_context_switch:
    /*
     * cdecl:
     * [esp + 4] = old_ctx
     * [esp + 8] = new_ctx
     */

    push ebp
    push ebx
    push esi
    push edi

    /*
     * Four pushes moved the arguments by 16 bytes.
     */
    mov eax, [esp + 20]       /* old_ctx */
    mov [eax], esp            /* old_ctx->esp = esp */

    mov eax, [esp + 24]       /* new_ctx */
    mov esp, [eax]            /* esp = new_ctx->esp */

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret

.size arch_context_switch, . - arch_context_switch
