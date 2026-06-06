#include "fs/procfs/procfs.h"

fs_node_t* proc_dir;
static uint32_t proc_inode_count = 0;

fs_dirent_t* proc_readdir(fs_node_t* node, uint32_t index);
fs_node_t* proc_parent(fs_node_t* node);
int32_t proc_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
fs_node_t* proc_finddir(fs_node_t* node, char* name);

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

fs_node_t* create_node_from_proc_file(proc_file_t* proc, uint32_t inode) {
    if (!proc)
        return NULL;

    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node) {
        kfree(node);
        return NULL;
    }

    strcpy(node->name, proc->name);
    node->flags = FS_FILE | FS_PROC;
    node->inode = inode;
    node->size = 0;
    node->device = proc;
    
    node->read = proc_read;
    node->readdir = NULL;
    node->finddir = NULL;
    node->parent = proc_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;
    
    return node;
}

fs_node_t* create_node_from_proc_dir(proc_dir_t* dir, uint32_t inode) {
    if (!dir)
        return NULL;

    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node) {
        kfree(node);
        return NULL;
    }
        

    strcpy(node->name, dir->name);
    node->flags = FS_DIR | FS_PROC;
    node->inode = inode;
    node->size = 0;
    node->device = dir;

    node->read = NULL;
    node->readdir = proc_readdir;
    node->finddir = proc_finddir;
    node->parent = proc_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;

    return node;
}

fs_node_t* create_proc_file_from_process(fs_node_t* parent, process_t* proc, uint32_t inode) {
    if (!proc)
        return NULL;

    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node)
        return NULL;
    
    uint_to_str(proc->pid, node->name);
    node->flags = FS_FILE | FS_PROC;
    node->inode = inode;
    node->size = 0;
    node->device = create_proc_file(parent, proc->pid, inode);
    
    node->read = proc_read;
    node->readdir = NULL;
    node->finddir = NULL;
    node->parent = proc_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;
    
    return node;
}

fs_node_t* create_root_dir(const char* name, fs_node_t* root, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    proc_dir_t* dir = kzmalloc(sizeof(proc_dir_t));

    if (!node || !dir) {
        kfree(node);
        kfree(dir);
        return NULL;
    }
        

    strcpy(node->name, name);
    node->flags = FS_DIR | FS_PROC;
    node->inode = inode;
    node->size = 0;
    node->device = dir;
    
    strcpy(dir->name, name);
    dir->child_count = 0;
    dir->parent = root;

    node->read = NULL;
    node->readdir = proc_readdir;
    node->finddir = proc_finddir;
    node->parent = proc_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;

    return node;
}

int proc_add_child(fs_node_t* dir, proc_file_t* child) {
    if (!dir || !child) return 0;

    proc_dir_t* proc = (proc_dir_t*)dir->device;
    if (!proc) return 0;

    if (proc->child_count >= MAX_PROCESSES) return 0;

    proc->children[proc->child_count++] = child;

    return 1;
}

fs_dirent_t* proc_readdir(fs_node_t* node, uint32_t index) {
    proc_dir_t* dir = (proc_dir_t*)node->device;
    if (!dir)
        return NULL;
    
    uint32_t seen = 0;
    proc_file_t* file = NULL;
    fs_dirent_t* dent = kzmalloc(sizeof(fs_dirent_t));
    if (!dent)
        return NULL;

    for (uint32_t i = 0; i < dir->child_count; i++, seen++) {
        if (seen == index) {
            file = dir->children[i];
            
            strcpy(dent->name, file->name);
            dent->inode = file->inode;

            return dent;
        } 
    }
    
    for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i])
            continue;
        
        if (seen == index) {
            uint32_t pid = process_table[i]->pid;
            uint_to_str(pid, dent->name);
            dent->inode = pid;
            
            return dent;
        }

        seen++;
    }
    
    kfree(dent);
    return NULL;
}

fs_node_t* proc_parent(fs_node_t* node) {
    if (!node) return NULL;

    if (!(node->flags & FS_PROC)) return NULL;

    if (node->flags & FS_DIR) {
        proc_dir_t* proc = (proc_dir_t*)node->device;
        if (!proc)
            return NULL; 

        return fs_node_clone(proc->parent);
    } else {
        proc_file_t* proc = (proc_file_t*)node->device;
        if (!proc)
            return NULL;

        return fs_node_clone(proc->parent);
    }
}

int32_t proc_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !buffer)
        return 0;
    
    if ((node->flags & (FS_FILE | FS_PROC)) != (FS_FILE | FS_PROC))
        return 0;
    
    proc_file_t* file = (proc_file_t*)node->device;
    if (!file)
        return 0;
    
    int32_t len = file->get_data(file, buffer, offset, size);    
    
    return len;
}

fs_node_t* proc_finddir(fs_node_t* node, char* name) {
    if (!node || !name)
        return NULL; 
    
    if ((node->flags & (FS_DIR | FS_PROC)) != (FS_DIR | FS_PROC))
        return NULL;

    proc_dir_t* dir = (proc_dir_t*)node->device;
    if (!dir)
        return NULL;

    for (uint32_t i = 0; i < dir->child_count; i++) {
        if (strcmp(dir->children[i]->name, name) == 0) {
            return create_node_from_proc_file(dir->children[i], proc_inode_count++); // CHANGE INODE        
        }
    } 


    // get pid from name
    uint32_t pid;
    if (!str_to_uint(name, &pid)) {
        return NULL;
    }
    
    process_t* process = process_table[pid];
    if (!process) 
        return NULL;

    // create fs_node_t from process
    return create_proc_file_from_process(node, process, proc_inode_count++);
}


fs_node_t* init_proc() {
    proc_dir = create_root_dir("proc", fs_root, inode_count++);
    
    proc_file_t* cpuinfo = create_cpuinfo_file(proc_dir, inode_count++);
    proc_add_child(proc_dir, cpuinfo);

    return proc_dir;
}

