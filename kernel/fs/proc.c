#include "fs/proc.h"

fs_node_t* proc_dir;

fs_dirent_t* proc_readdir(fs_node_t* node, uint32_t index) {
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t));
    proc_dir_t* proc = (proc_dir_t*)node->proc;

    if (!proc || index > proc->child_count) return NULL;

    fs_node_t* child = proc->children[index];
    strcpy(dirent->name, child->name);
    dirent->inode = child->inode;

    return dirent;
}

fs_node_t* proc_finddir(fs_node_t* node, char* name) {
    if (!node || !name) return NULL; 

    proc_dir_t* proc = (proc_dir_t*)node->proc;
    if (!proc) return NULL;

    for (uint32_t i = 0; i < proc->child_count; i++) {
        fs_node_t* child = proc->children[i];
        if (strcmp(child->name, name) == 0) {
            return child; 
        }
    }

    return NULL;
}

fs_node_t* proc_parent(fs_node_t* node) {
    if (!node) return NULL;

    proc_dir_t* proc = (proc_dir_t*)node->proc;
    if (!proc) return NULL; 

    return proc->parent;
}

fs_node_t* create_proc_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    proc_dir_t* data = kzmalloc(sizeof(proc_dir_t));

    if (!node || !data) {
        log_error("node is at %p and data is at %p\n", node, data);
        return NULL;
    }
        

    strcpy(node->name, name);
    node->flags = FS_DIR;
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

    proc_dir_t* proc = (proc_dir_t*)dir->proc;
    if (!proc) return 0;

    proc->children[proc->child_count++] = child;

    return 1;
}

void init_proc() {
    proc_dir = create_proc_dir("proc", fs_root, num_nodes++);
    
    for (uint32_t i = 0; i < num_processes; i++) {
        process_t* proc = process_table[i];
        fs_node_t* proc_node = kzmalloc(sizeof(fs_node_t));
        strcpy(proc_node->name, proc->name); 
        proc_node->flags = FS_PROC;
        proc_node->inode = num_nodes++;

        proc_add_child(proc_dir, proc_node);
    }
}


