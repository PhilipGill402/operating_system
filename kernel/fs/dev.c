#include "fs/dev.h"

fs_node_t* dev_dir;

fs_dirent_t* dev_readdir(fs_node_t* node, uint32_t index) {
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t)); 
    dev_dir_t* dir = (dev_dir_t*)node->device;
    
    if (!dir || index >= dir->child_count) {
        log_error("dir is at %p or index >= dir->child_count\n", node);
        return NULL;
    }
        

    fs_node_t* child = dir->children[index];
    strcpy(dirent->name, child->name);
    dirent->inode = child->inode;

    return dirent;
}

fs_node_t* dev_finddir(fs_node_t* node, char* name) {
    if (!node || !name) return NULL; 

    dev_dir_t* dir = (dev_dir_t*)node->device;

    if (!dir) {
        log_error("dir is at null\n");
        return NULL;
    } 
        

    for (uint32_t i = 0; i < dir->child_count; i++) {
        if (strcmp(dir->children[i]->name, name) == 0) {
            return dir->children[i];
        }
    }

    return NULL;
}

fs_node_t* dev_parent(fs_node_t* node) {
    if (!node || !node->device) {
        log_error("node is at %p or node->device is\n", node);
        return NULL;
    }
        
    
    dev_dir_t* dir = node->device;
    
    return dir->parent;
}

fs_node_t* create_dev_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    dev_dir_t* data = kzmalloc(sizeof(dev_dir_t));

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
    node->readdir = dev_readdir;
    node->finddir = dev_finddir;
    node->parent = dev_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;

    return node;
}

int dev_add_child(fs_node_t* dir, fs_node_t* child) {
    if (!dir || !child) {
        log_error("dir is at %x and child is at %x\n", dir, child);
        return 0;
    }
        

    dev_dir_t* device = dir->device;
    
    if (!device || device->child_count >= 16) {
        log_error("device is at %x or device->child_count >= 16\n", device);
        return 0;
    }
        

    device->children[device->child_count++] = child;

    return 1;
}

void init_dev() {
    dev_dir = create_dev_dir("dev", fs_root, num_nodes);
    console_node = create_console_node(dev_dir);

    if (!dev_add_child(dev_dir, console_node))
        log_error("failed to init devices\n");
}

