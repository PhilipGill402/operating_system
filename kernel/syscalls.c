#include "syscalls.h"

static int copy_argv(const char* argv[], cmd_args_t* args) {
    args->argc = 0;

    if (!argv) 
        return 1;

    for (uint32_t i = 0; i < MAX_ARGV; i++) {
        if (!argv[i])
            break;

        size_t len = strlen(argv[i]) + 1;
        char* copy = kmalloc(len);
        if (!copy)
            return 0;
        
        memcpy(copy, argv[i], len);

        args->argv[args->argc++] = copy;
    }

    args->argv[args->argc] = NULL;
    return 1;
}

static void free_exec_args(cmd_args_t* args) {
    for (uint32_t i = 0; i < args->argc; i++) {
        kfree(args->argv[i]);
    }
}

int sys_lseek(uint32_t fd, uint32_t offset) {
    current_process->fds[fd].offset = offset;

    return offset;
}

int sys_close(uint32_t fd) {
    if (fd >= MAX_FDS)
        return -1;

    kfree(current_process->fds[fd].node); 
    memset(&current_process->fds[fd], 0, sizeof(file_desc_t));

    return 1;
}

int sys_waitpid(uint32_t pid, int* status, int options) {
    (void)options;

    process_t* child = get_process(pid);

    if (!child) return -1;

    if (child->state == PROC_TERMINATED && !child->waited_on) {
        int child_status = child->exit_status;
        
        if (status) *status = child_status;

        child->waited_on = 1;

        process_destroy(child);

        return pid;
    }

    current_process->state = PROC_BLOCKED;
    current_process->waiting_for_pid = pid;
    current_process->waiting_status_ptr = status;

    schedule();
}

uint32_t sys_getdents(uint32_t fd, sys_dirent_t* dents, uint32_t count) {
    file_desc_t file_desc = current_process->fds[fd]; 
    fs_node_t* file = file_desc.node;
    
    if (!file || !(file->flags & FS_DIR)) return 0;
    
    uint32_t num_entries = 0;
    for (uint32_t i = 0; i < count; i++) {
        fs_dirent_t* fs_dent = fs_readdir(file, file_desc.offset++);
        if (!fs_dent) break;

        sys_dirent_t dent;
        strcpy(dent.name, fs_dent->name);
        dent.inode = fs_dent->inode;

        dents[num_entries++] = dent;
    }

    return num_entries;
}

uint32_t sys_open(const char* path, uint32_t flags) {
    fs_node_t* file = resolve_path_from(current_process->cwd, path);
    
    if (!file) return MAX_FDS + 1;
    
    // finding lowest available fd
    uint32_t fd = MAX_FDS + 1;
    for (uint32_t i = 0; i < MAX_FDS; i++) {
        if (current_process->fds[i].in_use == 0) {
            fd = i;
            break;
        }
    }

    if (fd >= MAX_FDS + 1) {
        log_error("Too many file descriptors open\n");
    }

    file_desc_t file_desc = {
        .node = file,
        .flags = flags,
        .offset = 0,
        .in_use = 1
    };

    current_process->fds[fd] = file_desc;

    return fd;
}

void* sys_brk(void* new_addr) {
    uint32_t addr = (uint32_t)new_addr;
    
    if (addr == 0) { 
        return (void*)current_process->heap_break;
    }
    
    if (addr < current_process->heap_start || addr > current_process->heap_max_end) {
        return NULL;
    }
    

    uint32_t* pd = temp_map_phys0(current_process->page_directory_phys);
    while (addr > current_process->heap_mapped_end) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return NULL;
        }

        map_user_page(pd, current_process->heap_mapped_end, frame, PAGE_WRITE);
        
        uint8_t* page = temp_map_phys1(frame);
        memset(page, 0, PAGE_SIZE);

        current_process->heap_mapped_end += PAGE_SIZE;
    }
    
    current_process->heap_break = addr;
    return (void*)addr;
}

uint32_t sys_read(uint32_t fd, char* buffer, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;
    
    uint32_t bytes_read = file->read(file, current_process->fds[fd].offset, count, (uint8_t*)buffer);
    current_process->fds[fd].offset += bytes_read;

    return bytes_read;
}

uint32_t sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;

    uint32_t bytes_written = file->writefile(file, str, current_process->fds[fd].offset, count);
    current_process->fds[fd].offset += bytes_written;

    return bytes_written;
}

uint32_t sys_fork() {
    process_t* new = process_clone(current_process);
    
    if (!new) return (uint32_t)-1;
    new->trapframe->eax = 0; // child process must return 0
    
    current_process->ticks_left = 1;
    int result = enqueue(&current_processes, &new);

    return new->pid;
}

uint32_t sys_execve(const char* file_name, const char* argv[]) {
    fs_node_t* elf = current_process->cwd->finddir(current_process->cwd, file_name);
    if (!elf) {
        return 0;
    }
    
    cmd_args_t args = { 0 };
    if (!copy_argv(argv, &args)) {
        kfree(elf);
        return 0;
    }

    if (!process_exec_from_elf(current_process, elf, &args)) {
        free_exec_args(&args); 
        kfree(elf);
        return 0;
    }
    
    free_exec_args(&args);
    kfree(elf);

    return 1;
}

uint32_t sys_exit(regs_t* reg, int32_t status) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    current_process->exit_status = status;
    
    log_debug("Process %d exited with status %d\n", current_process->pid, current_process->exit_status);

    process_wake_parent(current_process->pid);

    schedule();

    return 0;
}

uint32_t sys_chdir(const char* path) {
    fs_node_t* new_dir;
    if (!path) {
        new_dir = fs_root; 
    } else {
        new_dir = resolve_path_from(current_process->cwd, path);
        if (!new_dir || new_dir->flags != FS_DIR) return 0;
    }

    current_process->cwd = new_dir;
    
    return 1;
}

uint32_t sys_getcwd(char* buffer, size_t size) {
    if (!buffer || size == 0) return 0;

    fs_node_t* start = current_process->cwd;
    if (!start) return 0;

    fs_node_t* path[10];
    int idx = 0;

    buffer[0] = '\0';

    while (start) {
        path[idx++] = start;
        if (start->inode == fs_root->inode) {
            break;
        }

        start = fs_parent(start);
    }

    if (!start) return 0;

    size_t used = 0;
    
    for (int i = idx - 1; i >= 0; i--) {
        const char* name = path[i]->name;
        
        if (i == idx - 1) {
            // root
            if (used + 1 >= size) return 0;
            buffer[used++] = '/';
            buffer[used] = '\0';
            continue;
        }

        size_t len = strlen(name);

        if (used + len + 1 >= size) return 0;  // +1 for possible slash or null

        if (used > 1) {
            buffer[used++] = '/';
        }

        memcpy(buffer + used, name, len);
        used += len;
        buffer[used] = '\0';
    }
    
    return used;
}

uint32_t sys_getpid() {
    return current_process->pid;
}

void syscall_handler(regs_t* reg) {
    uint32_t ret = 0;
    current_process->trapframe = reg;
    
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
            ret = sys_exit(reg, (int32_t)reg->ebx);
            break;
        case SYS_GETCWD:
            ret = sys_getcwd((char*)reg->ebx, (size_t)reg->ecx);
            break;
        case SYS_CHDIR:
            ret = sys_chdir((char*)reg->ebx);
            break;
        case SYS_GETPID:
            ret = sys_getpid();
            break;
        case SYS_BRK:
            ret = (uint32_t)sys_brk((void*)reg->ebx);
            break;
        case SYS_OPEN:
            ret = sys_open((char*)reg->ebx, reg->ecx);
            break;
        case SYS_GETDENTS:
            ret = sys_getdents(reg->ebx, (sys_dirent_t*)reg->ecx, reg->edx);
            break;
        case SYS_WAITPID:
            ret = sys_waitpid(reg->ebx, (int*)reg->ecx, reg->edx);
            break;
        case SYS_CLOSE:
            ret = sys_close(reg->ebx);
            break;
        case SYS_LSEEK:
            ret = sys_lseek(reg->ebx, reg->ecx);
            break;
    }

    reg->eax = ret;
    reg->eflags |= 0x200;
    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}


