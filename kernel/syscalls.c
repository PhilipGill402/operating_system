#include "syscalls.h"

static inline uint32_t read_cr3(void) {
    uint32_t cr3;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(cr3)
    );

    return cr3;
}

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

static uint32_t vma_is_range_free(process_t* proc, uint32_t start, uint32_t end) {
    if (!proc)
        return 0;

    if (start >= end)
        return 0;

    if (start < USER_MMAP_BASE || end > USER_MMAP_END)
        return 0;

    vm_area_t* vma = proc->vmas;

    while (vma) {
        if (start < vma->end && vma->start < end)
            return 0;

        vma = vma->next;
    }

    return 1;
}

static uint32_t vma_find_free_range(process_t* proc, uint32_t length) {
    length = PAGE_ALIGN_UP(length);

    for (uint32_t addr = USER_MMAP_BASE; addr + length < USER_MMAP_END; addr += PAGE_SIZE) {
        if (addr + length < addr)
            return 0;

        if (vma_is_range_free(proc, addr, addr + length))
            return addr;
    }
    
    return 0;
}

static uint32_t mmap_prot_to_page_flags(prot) {
    uint32_t page_flags = PAGE_USER | PAGE_PRESENT;

    if (prot & PROT_WRITE) {
        page_flags |= PAGE_WRITE;
    }

    return page_flags;
}

static vm_area_t* vma_remove_exact(process_t* proc, uint32_t start, uint32_t end) {
    vm_area_t* prev = NULL; 
    vm_area_t* vm = proc->vmas;
    
    while (vm) {
        if (vm->start == start && vm->end == end) {
            if (prev)
                prev->next = vm->next;
            else
                proc->vmas = vm->next;

            vm->next = NULL;
            return vm;
        }
        
        prev = vm;
        vm = vm->next;
    }

    return NULL;
}

int32_t sys_close(uint32_t fd) {
    if (fd >= MAX_FDS)
        return EBADF;

    file_desc_t* desc = &current_process->fds[fd];

    if (!desc->in_use)
        return EBADF;

    if (desc->node) {
        kfree(desc->node);
    }
    
    memset(desc, 0, sizeof(file_desc_t));

    return 1;
}

int32_t sys_dup(uint32_t old_fd) {
    if (old_fd >= MAX_FDS || !current_process->fds[old_fd].in_use)
        return EBADF;
    
    file_desc_t old_file_desc = current_process->fds[old_fd]; 

    // find first open file descriptor
    uint32_t fd = MAX_FDS + 1;
    for (uint32_t i = 0; i < MAX_FDS; i++) {
        if (current_process->fds[i].in_use == 0) {
            fd = i;
            break;
        }
    }

    if (fd >= MAX_FDS + 1)
        return ENFILE;
    
    file_desc_t file_desc = {
        .node = fs_node_clone(old_file_desc.node),
        .flags = old_file_desc.flags,
        .offset = old_file_desc.offset,
        .in_use = 1
    };
    
    current_process->fds[fd] = file_desc;

    return fd;
}

int32_t sys_dup2(uint32_t old_fd, uint32_t new_fd) {
    if (old_fd >= MAX_FDS || !current_process->fds[old_fd].in_use)
        return EBADF;
    
    if (old_fd == new_fd)
        return new_fd;

    if (new_fd >= MAX_FDS)
        return EBADF;

    file_desc_t old_file_desc = current_process->fds[old_fd];
    
    // ignore errors
    sys_close(new_fd);

    file_desc_t file_desc = { 
        .node = fs_node_clone(old_file_desc.node),
        .flags = old_file_desc.flags,
        .offset = old_file_desc.offset,
        .in_use = 1
    };

    current_process->fds[new_fd] = file_desc;

    return new_fd;
}

int32_t sys_fb_flush(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (x + width >= framebuffer.width)
        width = framebuffer.width - x;
    if (y + width >= framebuffer.height)
        height = framebuffer.height - y;

    for (uint32_t row = 0; row < height; row++) {
        uint8_t* dst = framebuffer.addr + (y + row) * framebuffer.pitch + x * 4;
        uint8_t* src = (uint8_t*)framebuffer.backbuffer + ((y + row) * framebuffer.width + x) * 4;

        memcpy(dst, src, width * 4);
    }

    return 1;
}

int32_t sys_fb_info(sys_fb_info_t* info) {
    if (!info)
        return EFAULT;

    info->width = fb_shared_buffer.width;
    info->height = fb_shared_buffer.height;
    info->pitch = fb_shared_buffer.pitch;
    info->bpp = fb_shared_buffer.bpp;
    info->size = fb_shared_buffer.size;

    return 1;
}

int32_t sys_munmap(void* addr, uint32_t length) {
    if (!addr || length == 0)
        return -1;

    if (addr < USER_MMAP_BASE || addr > USER_MMAP_END)
        return -1;

    process_t* proc = current_process;
    if (!proc)
        return -1;

    uint32_t start = (uint32_t)addr;
    if (start & (PAGE_SIZE - 1))
        return -1;

    uint32_t aligned_length = PAGE_ALIGN_UP(length);
    uint32_t end = start + aligned_length;

    vm_area_t* area = vma_remove_exact(proc, start, end);
    if (!area)
        return -1;
    
    uint32_t* pd = temp_map_phys1(proc->page_directory_phys);
    
    for (uint32_t i = 0; i < area->frame_count; i++) {
        uint32_t virt = area->start + i * PAGE_SIZE;

        uint32_t frame = unmap_page_from(pd, virt);

        if (area->frames[i] && area->owns_frames)
            pmm_free_frame(area->frames[i]);
    }

    kfree(area->frames);
    kfree(area);

    return 0;
}

static void* mmap_framebuffer(void* addr, uint32_t length, int32_t prot, int32_t flags) {
    process_t* proc = current_process;

    if (fb_shared_buffer.owner_pid != -1 && fb_shared_buffer.owner_pid != proc->pid)
        return NULL;
    
    uint32_t rounded_length = PAGE_ALIGN_UP(fb_shared_buffer.size);
    if (rounded_length != length)
        return NULL;

    if (length > rounded_length)
        return NULL;

    uint32_t start;

    if (flags & MAP_FIXED) {
        start = (uint32_t)addr;

        if (start != USER_FB_VADDR) {
            return NULL;
        }
    } 
    /*
    else {
        start = vma_find_free_range(proc, rounded_length);

        if (!start) {
            return NULL;
        }
    }
    */

    uint32_t end = start + rounded_length;

    if (!vma_is_range_free(proc, start, end))
        return NULL;

    vm_area_t* area = kmalloc(sizeof(vm_area_t));

    if (!area)
        return NULL;

    memset(area, 0, sizeof(vm_area_t));

    area->start = start;
    area->end = end;
    area->prot = prot;
    area->flags = flags;
    area->type = VMA_FRAMEBUFFER;
    area->frames = fb_shared_buffer.frames;
    area->frame_count = fb_shared_buffer.frame_count;
    area->owns_frames = 0;

    uint32_t page_flags = mmap_prot_to_page_flags(prot);
    
    uint32_t* pd = temp_map_phys0(proc->page_directory_phys); 
    for (uint32_t i = 0; i < fb_shared_buffer.frame_count; i++) {
        map_user_page(pd, start + i * PAGE_SIZE, fb_shared_buffer.frames[i], page_flags);
    }
    
    fb_shared_buffer.owner_pid = proc->pid;
    
    area->next = proc->vmas;
    proc->vmas = area;
    
    return (void*)start;
}

void* sys_mmap(void* addr, uint32_t length, int32_t prot, int32_t flags, int32_t fd, uint32_t offset) {
    (void)fd;
    (void)offset;

    if (length == 0)
        return NULL;

    process_t* proc = current_process;
    
    if ((flags & MAP_FRAMEBUFFER) == MAP_FRAMEBUFFER)
        return mmap_framebuffer(addr, length, prot, flags);

    uint32_t* pd = temp_map_phys1(proc->page_directory_phys);

    if (!proc || !pd) {
        return NULL;
    }

    uint32_t rounded_length = PAGE_ALIGN_UP(length);

    if (rounded_length == 0) {
        return NULL;
    }
    
    uint32_t start;
    if (flags & MAP_FIXED) {
        // TODO IMPLEMENT THIS
        start = vma_find_free_range(proc, rounded_length);
    } else {
        start = vma_find_free_range(proc, rounded_length);

        if (!start)
            return NULL;
    }

    uint32_t end = start + rounded_length;
    uint32_t frame_count = rounded_length / PAGE_SIZE;
    

    vm_area_t* area = kmalloc(sizeof(vm_area_t));
    area->start = start;
    area->end = end;
    area->prot = prot;
    area->flags = flags;
    area->type = VMA_ANON;
    area->frames = kmalloc(frame_count * sizeof(uint32_t));
    area->frame_count = frame_count;
    area->next = NULL;
    area->owns_frames = 1;

    uint32_t page_flags = mmap_prot_to_page_flags(prot);
    
    for (uint32_t i = 0; i < frame_count; i++) {
        uint32_t frame = pmm_alloc_frame();
        area->frames[i] = frame;

        map_user_page(pd, start + i * PAGE_SIZE, frame, page_flags);

        void* tmp = temp_map_phys0(frame);
        memset(tmp, 0, PAGE_SIZE);
    }

    area->next = proc->vmas;
    proc->vmas = area;

    return (void*)start;
}

int32_t sys_kill(uint32_t pid, int32_t sig) {
    if (!(sig & SIGTERM) && !(sig & SIGKILL) && !(sig & SIGSTOP) && !(sig & SIGCONT))
        return EINVAL;

    process_t* proc = get_process(pid);

    if (!proc)
        return ESRCH;
    
    proc->pending_signals |= sig;

    return 0;
}

int32_t sys_lseek(uint32_t fd, uint32_t offset) {
    current_process->fds[fd].offset = offset;

    return offset;
}

int32_t sys_waitpid(uint32_t pid, int* status, int options) {
    (void)options;
    
    while (1) {
        process_t* child = get_process(pid);

        if (!child)
            return ESRCH;
        
        if (child->state == PROC_TERMINATED && !child->waited_on) {
            int child_status = child->exit_status;
            
            if (status)
                *status = child_status;

            child->waited_on = 1;

            process_destroy(child);

            return pid;
        }

        current_process->state = PROC_BLOCKED;
        current_process->waiting_for_pid = pid;
        current_process->waiting_status_ptr = status;

        schedule_and_enter();
    }

    return ECHILD;
}

int32_t sys_getdents(uint32_t fd, sys_dirent_t* dents, uint32_t count) {
    file_desc_t file_desc = current_process->fds[fd]; 
    fs_node_t* file = file_desc.node;
    
    if (!file) return EBADF;
    if (!(file->flags & FS_DIR)) return ENOTDIR;
    
    uint32_t num_entries = 0;
    for (uint32_t i = 0; i < count; i++) {
        fs_dirent_t* fs_dent = fs_readdir(file, file_desc.offset++);
        if (!fs_dent) 
            continue;

        sys_dirent_t dent;
        strcpy(dent.name, fs_dent->name);
        dent.inode = fs_dent->inode;

        dents[num_entries++] = dent;

        kfree(fs_dent);
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
    if (!buffer) return EFAULT; 

    if (fd >= MAX_FDS) return EBADF;

    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file) return ENOENT;
    
    int32_t bytes_read = fs_read(file, current_process->fds[fd].offset, count, (uint8_t*)buffer);
    
    // TODO: Implement this later
    /*
    if (bytes_read == EAGAIN) {
        current_process->state = PROC_BLOCKED;
        current_process->interrupted_by_signal = 0;
        
        enqueue(&current_processes, &current_process);
        schedule_and_enter();

        if (current_process->interrupted_by_signal) {
            current_process->interrupted_by_signal = 0;
            return EINTR;
        }
    }
    */
    
    current_process->fds[fd].offset += bytes_read;

    return bytes_read;
}

int32_t sys_write(uint32_t fd, char* str, size_t count) {
    if (fd >= MAX_FDS)
        return EBADF;
    
    fs_node_t* file = current_process->fds[fd].in_use == 1 ? current_process->fds[fd].node : NULL;
    
    if (!file)
        return ENOENT;
    
    //str[count] = '\0';

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
    fs_node_t* elf = resolve_path(file_name, current_process->cwd); 
    
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
    memcpy(current_process->trapframe, reg, sizeof(regs_t)); 
    current_process->saved_kernel_esp = (uint32_t)reg;
    current_process->state = PROC_TERMINATED;
    current_process->exit_status = status;
    
    log_debug("Process %d (%s) exited with status %d\n", current_process->pid, current_process->name, current_process->exit_status);

    process_wake_parent(current_process->pid);

    schedule_and_enter();

    return 0;
}

int32_t sys_chdir(const char* path) {
    fs_node_t* new_dir;
    if (!path) {
        new_dir = fs_root; 
    } else {
        new_dir = resolve_path_from(current_process->cwd, path);
        
        if (!new_dir) return ENOENT;
        if (!(new_dir->flags & FS_DIR)) return ENOTDIR;
    }

    current_process->cwd = new_dir;
    
    return 0;
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
    int32_t ret = 0;
    memcpy(current_process->trapframe, reg, sizeof(regs_t));
     
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
            ret = sys_execve((char*)reg->ebx, (char**)reg->ecx);
            
            if (ret >= 0) {
                memcpy(reg, current_process->trapframe, sizeof(regs_t));
                return;
            }
            
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
            ret = (int32_t)sys_brk((void*)reg->ebx);
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
        case SYS_KILL:
            ret = sys_kill(reg->ebx, reg->ecx);
            break;
        case SYS_MMAP:
            ret = (int32_t)sys_mmap((void*)reg->ebx, reg->ecx, (int32_t)reg->edx, (int32_t)reg->esi, (int32_t)reg->edi, reg->ebp);
            break;
        case SYS_MUNMAP:
            ret = sys_munmap((void*)reg->ebx, reg->ecx);
            break;
        case SYS_FB_INFO:
            ret = sys_fb_info((sys_fb_info_t*)reg->ebx);
            break;
        case SYS_FB_FLUSH:
            ret = sys_fb_flush(reg->ebx, reg->ecx, reg->edx, reg->esi);
            break;
        case SYS_YIELD:
            reg->eax = 0;
            schedule_from_interrupt(reg);
            return;
        case SYS_DUP2:
            ret = sys_dup2(reg->ebx, reg->ecx);
            break;
        case SYS_DUP:
            ret = sys_dup(reg->ebx);
            break;

    }

    reg->eax = ret;
    reg->eflags |= 0x200;

    check_pending_signals(current_process);
    
    if (current_process->state != PROC_RUNNING) {
        schedule_from_interrupt(reg);
        return;
    }

}


