#include "fs/proc.h"

fs_node_t* proc_dir;

static const char* proc_state_to_str(proc_state_t state) {
    switch (state) {
        case PROC_RUNNING: return "RUNNING";
        case PROC_READY: return "READY";
        case PROC_BLOCKED: return "BLOCKED";
        case PROC_TERMINATED: return "ZOMBIE";
        default: return "UNKNOWN";
    }
}

static void uint_to_str(uint32_t num, char* str) {
    int i = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

static int str_to_uint(const char* str, uint32_t* out) {
    if (!str || !*str) return 0;

    uint32_t result = 0;

    for (uint32_t i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }

        result = result * 10 + (str[i] - '0');
    }

    *out = result;
    return 1;
}

fs_dirent_t* proc_readdir(fs_node_t* node, uint32_t index) {
    uint32_t seen = 0; 
    for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
        process_t* proc = process_table[i];

        if (!proc) continue;

        if (seen == index) {
            fs_dirent_t* dent = kzmalloc(sizeof(fs_dirent_t));
            if (!dent) return NULL;

            uint_to_str(proc->pid, dent->name);
            dent->inode = proc->pid;
            
            return dent;
        }

        seen++;
    }

    return NULL;
}

fs_node_t* proc_parent(fs_node_t* node) {
    if (!node) return NULL;

    if (!(node->flags & FS_PROC)) return NULL;

    if (node->flags & FS_DIR) {
        proc_dir_t* proc = (proc_dir_t*)node->device;
        if (!proc) return NULL; 

        return fs_node_clone(proc->parent);
    } else {
        return fs_node_clone(proc_dir);
    }

    
}

int32_t proc_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !buffer) return 0;
    if ((node->flags & (FS_FILE | FS_PROC)) != (FS_FILE | FS_PROC)) return 0;

    process_t* proc = process_table[node->inode];
    if (!proc) return 0;
    
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

fs_node_t* proc_finddir(fs_node_t* node, char* name) {
    if (!node || !name) return NULL; 

    // get pid from name
    uint32_t pid;
    if (!str_to_uint(name, &pid)) {
        return NULL;
    }
    process_t* process = process_table[pid];
    if (!process) return NULL;

    // create fs_node_t from process
    fs_node_t* proc_node = kzmalloc(sizeof(fs_node_t));
    if (!proc_node) return NULL;

    strcpy(proc_node->name, name);
    proc_node->inode = pid;
    proc_node->flags = FS_PROC | FS_FILE;
    proc_node->size = 0;

    proc_node->read = proc_read;
    proc_node->readdir = NULL;
    proc_node->finddir = NULL;
    proc_node->parent = proc_parent;
    proc_node->createdir = NULL;
    proc_node->createfile = NULL;
    proc_node->writefile = NULL;

    return proc_node;
}



fs_node_t* create_proc_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    proc_dir_t* data = kzmalloc(sizeof(proc_dir_t));

    if (!node || !data) {
        log_error("node is at %x and data is at %x\n", node, data);
        kfree(node);
        kfree(data);
        return NULL;
    }
        

    strcpy(node->name, name);
    node->flags = FS_DIR | FS_PROC;
    node->inode = inode;
    node->size = 0;
    node->device = data;

    data->child_count = 0;
    data->parent = parent;

    node->read = NULL;
    node->readdir = proc_readdir;
    node->finddir = proc_finddir;
    node->parent = proc_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;

    return node;
}

int proc_add_child(fs_node_t* dir, fs_node_t* child) {
    if (!dir || !child) return 0;

    proc_dir_t* proc = (proc_dir_t*)dir->device;
    if (!proc) return 0;

    if (proc->child_count >= MAX_PROCESSES) return 0;

    proc->children[proc->child_count++] = child;

    return 1;
}

fs_node_t* init_proc() {
    proc_dir = create_proc_dir("proc", fs_root, inode_count++);

    return proc_dir;
}


