#include "syscalls.h"

int32_t sys_read(uint32_t fd, char* buffer, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;

    return file->read(file, 0, count, (uint8_t*)buffer);
}

// TODO: implement stdin and stdout and rework this
int32_t sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;

    return file->writefile(file, str, 0, count);
}

int32_t sys_fork() {
    process_t* new = process_clone(current_process);
    
    if (!new) return 0;

    enqueue(&current_processes, &new);

    return 1;
}

// TODO: REWORK! THIS DOESNT WORK!
int32_t sys_execve(const char* file_name, const char* argv) {
    fs_node_t* elf = fs_cwd->finddir(fs_cwd, file_name);
    
    if (!elf) {
        kfree(elf);
        return -1;
    }

    memcpy(current_process, elf, sizeof(process_t)); 
    kfree(elf);

    return 1;
}

void sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();
}

void syscall_handler(regs_t* reg) {
    int32_t ret = 0;
    switch (reg->eax) {
        case SYS_READ:
            ret = sys_read(reg->ebx, (char*)reg->ecx, (size_t)reg->edx);
            break;
        case SYS_WRITE:
            ret = sys_write(reg->ebx, (char*)reg->ecx, (size_t)reg->edx);
            break;
        case SYS_FORK:
            ret = sys_fork(reg);
            break;
        case SYS_EXECVE:
            ret = sys_execve((char*)reg->ebx, (char*)reg->ecx);
            break;
        case SYS_EXIT:
            sys_exit(reg);
            break;
        
    }

    reg->eax = ret;

    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}
