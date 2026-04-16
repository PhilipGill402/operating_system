#include "syscalls.h"

void sys_read(uint32_t fd, char* buffer, size_t count) {
    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds->node : NULL;
    
    if (!file) {
        return;
    }

    file->read(file, 0, count, (uint8_t*)buffer);
}

// TODO: implement stdin and stdout and rework this
void sys_write(uint32_t fd, const char* str, size_t count) {
    printf("%s", str);
}

void sys_fork() {
    process_t* new = process_clone(current_process); 
    enqueue(&current_processes, &new);
}

void sys_execve(const char* file_name, const char* argv) {
    (void*)(file_name);
}

void sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();

    for (;;) {}
}

void syscall_handler(regs_t* reg) {
    switch (reg->eax) {
        case SYS_READ:
            sys_read(reg->ebx, (char*)reg->ecx, (size_t)reg->edx);
            break;
        case SYS_WRITE:
            sys_write(reg->ebx, (char*)reg->ecx, (size_t)reg->edx);
            break;
        case SYS_FORK:
            sys_fork(reg);
            break;
        case SYS_EXECVE:
            sys_execve((char*)reg->ebx, (char*)reg->ecx);
            break;
        case SYS_EXIT:
            sys_exit(reg);
            break;
        
    }

    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}
