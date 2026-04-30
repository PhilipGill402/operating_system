#include "syscalls.h"

void debug_dump_process_queue_logical(const char* label) {
    printf("\nQUEUE LOGICAL DUMP: %s\n", label);
    printf("size=%d capacity=%d head=%d rear=%d element_size=%d expected=%d\n",
           current_processes.size,
           current_processes.capacity,
           current_processes.head,
           current_processes.rear,
           current_processes.element_size,
           sizeof(process_t*));

    for (uint32_t i = 0; i < current_processes.size; i++) {
        uint32_t idx = (current_processes.head + i) % current_processes.capacity;

        process_t* p = NULL;
        memcpy(&p,
               (char*)current_processes.array + idx * current_processes.element_size,
               sizeof(process_t*));

        printf("logical %d physical slot %d: ptr=%x", i, idx, p);

        if (p) {
            printf(" pid=%d state=%d", p->pid, p->state);
        }

        if (i == 0) {
            printf(" <- next dequeue");
        }

        printf("\n");
    }

    printf("\n");
}

uint32_t sys_getdents(uint32_t fd, dirent_t* dents, uint32_t count) {
    file_desc_t file_desc = current_process->fds[fd]; 
    fs_node_t* file = file_desc.node;
    
    if (!file || file->flags != FS_DIR) return 0;
    
    uint32_t num_entries = 0;
    for (uint32_t i = 0; i < count; i++) {
        fs_dirent_t* fs_dent = file->readdir(file, file_desc.offset++);
        if (!fs_dent) break;

        dirent_t dent;
        strcpy(dent.name, fs_dent->name);
        dent.inode = fs_dent->inode;

        dents[num_entries++] = dent;
    }

    return num_entries;
}

uint32_t sys_open(const char* path, uint32_t flags) {
    fs_node_t* file = resolve_path_from(current_process->cwd, path);
    
    if (!file) return MAX_FDS + 1;

    uint32_t fd = current_process->open_fds++;

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
    current_process->fds[fd].offset = bytes_read;

    return bytes_read;
}

uint32_t sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS) return 0;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return 0;

    uint32_t bytes_written = file->writefile(file, str, current_process->fds[fd].offset, count);
    current_process->fds[fd].offset = bytes_written;

    return bytes_written;
}

extern uint8_t debug_sched;
uint32_t sys_fork() {
    process_t* new = process_clone(current_process);
    
    if (!new) return (uint32_t)-1;
    new->trapframe->eax = 0; // child process must return 0
    
    current_process->ticks_left = 1;
    int result = enqueue(&current_processes, &new);
    debug_sched = 1;    

    return new->pid;
}

uint32_t sys_execve(const char* file_name, const char* argv) {
    fs_node_t* elf = fs_cwd->finddir(fs_cwd, file_name);
    process_t* proc_elf = process_create_from_elf(elf);
    
    if (!proc_elf) {
        kfree(elf);
        kfree(proc_elf);
        return 0;
    }

    memcpy(current_process, proc_elf, sizeof(process_t));
    kfree(elf);
    kfree(proc_elf);

    return 1;
}

uint32_t sys_exit(regs_t* reg) {
    current_process->trapframe = reg;
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    schedule();

    return 1;
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
    
    for (int8_t i = idx - 1; i >= 0; i--) {
        kfree(path[i]);
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
            ret = sys_exit(reg);
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
            ret = sys_getdents(reg->ebx, (dirent_t*)reg->ecx, reg->edx);
            break;
    }

    reg->eax = ret;
    reg->eflags |= 0x200;
    enter_user_mode_from_trapframe(reg);
    __builtin_unreachable();
}


