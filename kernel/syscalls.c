#include "syscalls.h"

static char* trim_path(const char* path) {
    uint32_t last_slash = -1;

    for (uint32_t i = 0; i < strlen(path); i++) {
        if (path[i] == '/') 
            last_slash = i;
    }

    if (last_slash == -1) {
        return NULL;
    } else if (last_slash == 0) {
        char* trim = kmalloc(2);
        trim[0] = '/';
        trim[1] = '\0';

        return trim;
    }

    char* trim = kmalloc(last_slash + 1);
    strncpy(trim, path, last_slash);
    trim[last_slash] = '\0';

    return trim;
}

static char* get_file_name(const char* path) {
    char* file_name = kmalloc(strlen(path) + 1); 
    
    uint32_t idx = strlen(path);
    uint32_t count = 0;
    
    for (uint32_t idx = strlen(path); path[idx] != '/'; idx--) {
        file_name[count++] = path[idx];
    }
    
    // reverse string
    for (uint32_t i = 0; i <= count / 2; i++) {
        char tmp = file_name[i];
        file_name[i] = file_name[count - i - 1];
        file_name[count - i - 1] = tmp;
    } 

    return file_name;
}

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

int32_t sys_lseek(uint32_t fd, uint32_t offset) {
    current_process->fds[fd].offset = offset;

    return offset;
}

int32_t sys_close(uint32_t fd) {
    if (fd >= MAX_FDS)
        return EBADF;

    kfree(current_process->fds[fd].node); 
    memset(&current_process->fds[fd], 0, sizeof(file_desc_t));

    return 1;
}

int32_t sys_waitpid(uint32_t pid, int* status, int options) {
    (void)options;

    process_t* child = get_process(pid);

    if (!child) return ESRCH;

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

int32_t sys_getdents(uint32_t fd, sys_dirent_t* dents, uint32_t count) {
    file_desc_t file_desc = current_process->fds[fd]; 
    fs_node_t* file = file_desc.node;
    
    if (!file) return EBADF;
    if (!(file->flags & FS_DIR)) return ENOTDIR;
    
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

int32_t sys_open(const char* path, uint32_t flags, sys_mode_t mode) {
    fs_node_t* file = resolve_path(path, current_process->cwd);
    
    if (!file && flags & O_CREAT) {
        char* trim = trim_path(path);
        fs_node_t* dir = resolve_path(trim, current_process->cwd);
        if (!dir)
            return ENOENT;
        
        char* file_name = get_file_name(path);
        
        file = kmalloc(sizeof(fs_node_t));
        if (!file)
            return ENOMEM;

        fs_createfile(dir, file_name, 256);
        file = fs_finddir(dir, file_name);

        kfree(file_name);
        kfree(trim);
    } else if (!file) {
        return ENOENT;
    }

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
        return ENFILE; 
    }

    file_desc_t file_desc = {
        .node = file,
        .flags = flags,
        .offset = 0,
        .in_use = 1
    };

    if (flags & O_TRUNC)
        file_desc.offset = 0;
    else if (flags & O_APPEND)
        file_desc.offset = file->size;

    current_process->fds[fd] = file_desc;

    return fd;
}

void* sys_brk(void* new_addr) {
    uint32_t addr = (uint32_t)new_addr;
    
    if (addr == 0) { 
        return (void*)current_process->heap_break;
    }
    
    if (addr < current_process->heap_start || addr > current_process->heap_max_end) {
        return EFAULT;
    }
    

    uint32_t* pd = temp_map_phys0(current_process->page_directory_phys);
    while (addr > current_process->heap_mapped_end) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return EFAULT;
        }

        map_user_page(pd, current_process->heap_mapped_end, frame, PAGE_WRITE);
        
        uint8_t* page = temp_map_phys1(frame);
        memset(page, 0, PAGE_SIZE);

        current_process->heap_mapped_end += PAGE_SIZE;
    }
    
    current_process->heap_break = addr;
    return (void*)addr;
}

int32_t sys_read(uint32_t fd, char* buffer, size_t count) {
    if (fd >= MAX_FDS) return EBADF;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return ENOENT;
    
    uint32_t bytes_read = fs_read(file, current_process->fds[fd].offset, count, (uint8_t*)buffer);
    current_process->fds[fd].offset += bytes_read;

    return bytes_read;
}

int32_t sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS)
        return EBADF;
    
    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file)
        return ENOENT;
    
    uint32_t bytes_written = fs_writefile(file, str, current_process->fds[fd].offset, count);
    current_process->fds[fd].offset += bytes_written;

    return bytes_written;
}

int32_t sys_fork() {
    process_t* new = process_clone(current_process);
    
    // dont know if this is right, check later
    if (!new) return ESRCH;
    new->trapframe->eax = 0; // child process must return 0
    
    current_process->ticks_left = 1;
    int result = enqueue(&current_processes, &new);

    return new->pid;
}

int32_t sys_execve(const char* file_name, const char* argv[]) {
    fs_node_t* elf = current_process->cwd->finddir(current_process->cwd, file_name);
    
    if (!elf) return ENOENT;
    if (elf->flags == FS_DIR) return EISDIR;
    
    cmd_args_t args = { 0 };
    if (!copy_argv(argv, &args)) {
        kfree(elf);

        // this could be wrong too, check later
        return EFAULT;
    }

    if (!process_exec_from_elf(current_process, elf, &args)) {
        free_exec_args(&args); 
        kfree(elf);

        // this could be wrong too, check later
        return EFAULT;
    }
    
    free_exec_args(&args);
    kfree(elf);

    return 1;
}

int32_t sys_exit(regs_t* reg, int32_t status) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    current_process->exit_status = status;
    
    log_debug("Process %d exited with status %d\n", current_process->pid, current_process->exit_status);

    process_wake_parent(current_process->pid);

    schedule();

    return 0;
}

int32_t sys_chdir(const char* path) {
    fs_node_t* new_dir;
    if (!path) {
        new_dir = fs_root; 
    } else {
        new_dir = resolve_path_from(current_process->cwd, path);
        if (!new_dir) return ENOENT;
        if (new_dir->flags != FS_DIR) return ENOTDIR;
    }

    current_process->cwd = new_dir;
    
    return 1;
}

int32_t sys_getcwd(char* buffer, size_t size) {
    if (!buffer) return EFAULT;
    if (size == 0) return ERANGE;

    fs_node_t* start = current_process->cwd;
    if (!start) return ENOENT;
    
    fs_node_t* path[10];
    int idx = 0;

    buffer[0] = '\0';

    while (start) {
        path[idx++] = start;
        if (strcmp(start->name, fs_root->name) == 0) {
            break;
        }

        start = fs_parent(start);
    }

    if (!start) return ENOENT;

    size_t used = 0;
    
    for (int i = idx - 1; i >= 0; i--) {
        const char* name = path[i]->name;
        
        if (i == idx - 1) {
            // root
            if (used + 1 >= size) return ERANGE;
            buffer[used++] = '/';
            buffer[used] = '\0';
            continue;
        }

        size_t len = strlen(name);

        if (used + len + 1 >= size) return ERANGE;  // +1 for possible slash or null

        if (used > 1) {
            buffer[used++] = '/';
        }

        memcpy(buffer + used, name, len);
        used += len;
        buffer[used] = '\0';
    }
    
    return used;
}

int32_t sys_getpid() {
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
            ret = sys_open((char*)reg->ebx, reg->ecx, (sys_mode_t)reg->edx);
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


