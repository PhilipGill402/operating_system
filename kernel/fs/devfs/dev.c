#include "fs/dev.h"

fs_node_t* dev_dir;

fs_dirent_t* dev_readdir(fs_node_t* node, uint32_t index) {
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t)); 
    dev_dir_t* dir = (dev_dir_t*)node->device;
    
    if (!dir || index >= dir->child_count) {
        log_error("dir is at %x or index >= dir->child_count\n", node);
        kfree(dirent);
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
    fs_node_t* ret = kmalloc(sizeof(fs_node_t));

    if (!dir) {
        log_error("dir is at null\n");
        kfree(ret);
        return NULL;
    } 
        
    for (uint32_t i = 0; i < dir->child_count; i++) {
        fs_node_t* child = dir->children[i];
        if (strcmp(child->name, name) == 0) {
            memcpy(ret, child, sizeof(fs_node_t));
            strcpy(ret->name, child->name); 

            return ret;
        }
    }
    
    kfree(ret);
    return NULL;
}

fs_node_t* dev_parent(fs_node_t* node) {
    if (!node || !node->device) {
        log_error("node is at %x or node->device is\n", node);
        return NULL;
    }
    
    dev_dir_t* dir = node->device;

    if (!dir) return NULL;
    
    fs_node_t* ret = kmalloc(sizeof(fs_node_t));
    memcpy(ret, dir->parent, sizeof(fs_node_t));
    strcpy(ret->name, dir->parent->name);
    
    return ret;
}

fs_node_t* create_dev_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    dev_dir_t* data = kzmalloc(sizeof(dev_dir_t));

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

fs_node_t* init_dev() {
    dev_dir = create_dev_dir("dev", fs_root, inode_count++);
    console_node = create_console_node(dev_dir);

    if (!dev_add_child(dev_dir, console_node))
        log_error("failed to init devices\n");

    return dev_dir;
}

