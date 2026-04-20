#include "syscalls.h"

int32_t sys_read(uint32_t fd, char* buffer, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;
     
    return file->read(file, 0, count, (uint8_t*)buffer);
}

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

int32_t sys_execve(const char* file_name, const char* argv) {
    fs_node_t* elf = fs_cwd->finddir(fs_cwd, file_name);
    process_t* proc_elf = process_create_from_elf(elf);
    
    if (!proc_elf) {
        kfree(elf);
        kfree(proc_elf);
        return -1;
    }

    memcpy(current_process, proc_elf, sizeof(process_t));
    kfree(elf);
    kfree(proc_elf);

    return 1;
}

int32_t sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();

    return 0;
}

int32_t sys_chdir(const char* path) {
    fs_node_t* new_dir;
    if (!path) {
        new_dir = fs_root; 
    } else {
        new_dir = resolve_path_from(current_process->cwd, path);
        if (new_dir->flags != FS_DIR) return -1;
    }

    current_process->cwd = new_dir;

    return 0;
}

int32_t sys_getcwd(char* buffer, size_t size) {
    fs_node_t* start = current_process->cwd;
    fs_node_t* path[10] = { start };
    int8_t idx = 1;
    int32_t bytes_written = 0;

    if (!start) return -1;

    while (start->inode != fs_root->inode) {
        start = fs_parent(start);

        if (!start) return -1;

        path[idx++] = start;
    }

    for (int i = idx - 1; i >= 0; i--) {
        size_t len = strlen(path[i]->name);
        if (len > size - 1) break;
        size -= len;
        bytes_written += len;

        strcat(buffer, path[i]->name); 
    }
    
    for (int8_t i = idx - 1; i >= 0; i--) {
        kfree(path[i]);
    }

    buffer[bytes_written++] = '\0';

    return bytes_written;
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
        case SYS_GETCWD:
            sys_getcwd((char*)reg->ebx, (size_t)reg->ecx);
            break;
        case SYS_CHDIR:
            sys_chdir((char*)reg->ebx);
            break;
    }

    reg->eax = ret;

    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}
