global arch_context_switch
.type arch_context_switch , @function

arch_context_switch:
    /*
     * cdecl arguments:
     * [esp + 4] = old_ctx
     * [esp + 8] = new_ctx
     */

    push ebp
    push ebx
    push esi
    push edi

    /*
     * Four pushes changed the argument offsets by 16 bytes.
     */
    mov eax, [esp + 20]       ; old_ctx
    mov [eax], esp            ; old_ctx->esp = current esp

    mov eax, [esp + 24]       ; new_ctx
    mov esp, [eax]            ; esp = new_ctx->esp

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret
