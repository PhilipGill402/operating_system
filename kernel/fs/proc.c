#include "fs/proc.h"

fs_node_t* proc_dir;

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

fs_node_t* proc_finddir(fs_node_t* node, char* name) {
    if (!node || !name) return NULL; 

    proc_dir_t* proc = (proc_dir_t*)node->proc;
    if (!proc) return NULL;

    for (uint32_t i = 0; i < proc->child_count; i++) {
        fs_node_t* child = proc->children[i];
        if (strcmp(child->name, name) == 0) {
            return fs_node_clone(child); 
        }
    }

    return NULL;
}

fs_node_t* proc_parent(fs_node_t* node) {
    if (!node) return NULL;

    proc_dir_t* proc = (proc_dir_t*)node->proc;
    if (!proc) return NULL; 

    return fs_node_clone(proc->parent);
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
    node->flags = FS_DIR;
    node->inode = inode;
    node->size = 0;
    node->proc = data;

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

    proc_dir_t* proc = (proc_dir_t*)dir->proc;
    if (!proc) return 0;

    if (proc->child_count >= MAX_PROCESSES) return 0;

    proc->children[proc->child_count++] = child;

    return 1;
}

void init_proc() {
    proc_dir = create_proc_dir("proc", fs_root, num_nodes++);
    
    for (uint32_t i = 0; i < num_processes; i++) {
        process_t* proc = process_table[i];
        if (!proc) continue;

        fs_node_t* proc_node = kzmalloc(sizeof(fs_node_t));
        if (!proc_node) continue;

        strcpy(proc_node->name, proc->name); 
        proc_node->flags = FS_PROC;
        proc_node->inode = num_nodes++;

        proc_add_child(proc_dir, proc_node);
    }
}


