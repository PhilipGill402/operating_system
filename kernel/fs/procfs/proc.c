#include "fs/procfs/proc.h"

static const char* proc_state_to_str(proc_state_t state) {
    switch (state) {
        case PROC_RUNNING: return "RUNNING";
        case PROC_READY: return "READY";
        case PROC_BLOCKED: return "BLOCKED";
        case PROC_TERMINATED: return "ZOMBIE";
        default: return "UNKNOWN";
    }
}

static int32_t get_proc_data(proc_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    if (!file || !buffer)
        return 0;
    
    process_t* proc = process_table[file->pid];
    if (!proc) 
        return 0;
    
    char intern_buffer[256];

    int32_t bytes_written = snprintf(intern_buffer, size, "PID: %d\tPPID: %d\tStatus: %s\tName: %s", proc->pid, proc->ppid, proc_state_to_str(proc->state), proc->name);
    intern_buffer[bytes_written] = '\0';
    
    size_t len = strlen(intern_buffer);

    if (offset >= len)
        return 0;

    bytes_written = offset + size < len ? offset + size : len - offset;
    memcpy(buffer, intern_buffer + offset, bytes_written);
    
    return bytes_written;    
}

proc_file_t* create_proc_file(fs_node_t* parent, uint32_t pid, uint32_t inode) {
    if (!parent)
        return NULL;

    proc_file_t* file = kmalloc(sizeof(proc_file_t));
    if (!file)
        return NULL;


    file->parent = fs_node_clone(parent);
    if (!file->parent)
        return NULL;
    int bytes_written = snprintf(file->name, sizeof(file->name), "%d", pid);
    file->name[bytes_written] = '\0';
    
    file->pid = pid;
    file->inode = inode;
    file->get_data = get_proc_data;

    return file;
}
