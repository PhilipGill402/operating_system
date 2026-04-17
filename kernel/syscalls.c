#include "syscalls.h"

uint32_t sys_read(uint32_t fd, char* buffer, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;

    return file->read(file, 0, count, (uint8_t*)buffer);
}

// TODO: implement stdin and stdout and rework this
void sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS) return;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return;

    file->writefile(file, str, 0, count);
}

void sys_fork() {
    process_t* new = process_clone(current_process); 
    enqueue(&current_processes, &new);
}

// TODO: REWORK! THIS DOESNT WORK!
void sys_execve(const char* file_name, const char* argv) {
    fs_node_t* elf = fs_cwd->finddir(fs_cwd, file_name);
    
    if (!elf) {
        kfree(elf);
        return;
    }

    elf_execute(elf); 
    kfree(elf);
}

void sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();

    for (;;) {}
}

void syscall_handler(regs_t* reg) {
    uint32_t ret = 0;
    switch (reg->eax) {
        case SYS_READ:
            ret = sys_read(reg->ebx, (char*)reg->ecx, (size_t)reg->edx);
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

    reg->eax = ret;

    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}
