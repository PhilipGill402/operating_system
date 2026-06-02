#include "fs/ramfs.h"

#include <log.h>

ramfs_node_t* root;

// forward declaration
fs_node_t* ramfs_to_vfs(ramfs_node_t* ramfs);

void ramfs_createdir(fs_node_t* node, char* name) {
    if (!node || !node->device)
        return;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    if (ramfs->type != FS_DIR)
        return;

    ramfs_node_t* child = kmalloc(sizeof(ramfs_node_t));

    if (!child)
        return;
    
    strcpy(child->name, name);
    child->type = FS_DIR;
    child->inode = inode_count++;
    child->parent = ramfs;
    child->child_count = 0;
    child->data = NULL;
    child->size = 0;
    child->capacity = 0;
    child->fs_node = ramfs_to_vfs(child);

    ramfs->children[ramfs->child_count++] = child;

}

void ramfs_createfile(fs_node_t* node, char* name, uint32_t size) {
    if (!node || !node->device)
        return;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    if (ramfs->type != FS_DIR)
        return;

    ramfs_node_t* child = kmalloc(sizeof(ramfs_node_t));

    if (!child)
        return;
    
    child->data = kmalloc(RAMFS_SIZE_MAX);
    
    if (!child->data)
        return;
    
    strcpy(child->name, name);
    child->type = FS_FILE;
    child->inode = inode_count++;
    child->parent = ramfs;
    child->child_count = 0;
    child->size = 0;
    child->capacity = size;
    child->fs_node = ramfs_to_vfs(child);

    ramfs->children[ramfs->child_count++] = child;
}

uint32_t ramfs_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    if (!node || !node->device)
        return 0;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;
    
    if (offset > ramfs->capacity)
        return 0;

    uint32_t write_size = offset + size < ramfs->capacity ? size : ramfs->capacity - offset;
    
    uint8_t* dst = ramfs->data + offset;
    memcpy((void*)dst, (void*)buffer, write_size);

    ramfs->size += write_size;

    return write_size;
}

uint32_t ramfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !node->device)
        return 0;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    if (offset > ramfs->capacity)
        return 0;

    uint32_t read_size = offset + size < ramfs->size ? size : ramfs->size - offset;

    uint8_t* src = ramfs->data + offset;
    memcpy((void*)buffer, (void*)src, read_size);

    return read_size;
}

fs_dirent_t* ramfs_readdir(fs_node_t* node, uint32_t index) {
    if (!node || !node->device) 
        return NULL;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    if (index >= ramfs->child_count)
        return NULL;

    ramfs_node_t* child = ramfs->children[index];

    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t));

    if (!dirent)
        return NULL;

    strcpy(dirent->name, child->name);
    dirent->inode = child->inode;
    
    return dirent;
}

fs_node_t* ramfs_finddir(fs_node_t* node, char* name) {
    if (!node || !node->device)
        return NULL;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    for (uint32_t i = 0; i < ramfs->child_count; i++) {
        ramfs_node_t* child = ramfs->children[i];

        if (strcmp(name, child->name) == 0) {
            return ramfs_to_vfs(child);
        }
    }

    return NULL;
}

fs_node_t* ramfs_parent(fs_node_t* node) {
    if (!node || !node->device)
        return NULL;

    ramfs_node_t* ramfs = (ramfs_node_t*)node->device;

    return ramfs_to_vfs(ramfs->parent);
}

fs_node_t* ramfs_to_vfs(ramfs_node_t* ramfs) {
    fs_node_t* node = kmalloc(sizeof(fs_node_t));
    if (!node)
        return NULL;

    strcpy(node->name, ramfs->name);
    node->flags = ramfs->type;
    node->inode = ramfs->inode;
    node->size = ramfs->size;
    node->mount_parent = NULL;
    node->device = ramfs;
    
    node->createdir = ramfs_createdir;
    node->createfile = ramfs_createfile;
    node->writefile = ramfs_writefile;
    node->read = ramfs_read;
    node->readdir = ramfs_readdir;
    node->finddir = ramfs_finddir;
    node->parent = ramfs_parent;

    return node;
}

fs_node_t* ramfs_init() {
    root = kmalloc(sizeof(ramfs_node_t));
    
    if (!root)
        return NULL;

    strcpy(root->name, "tmp"); 
    root->type = FS_DIR;
    root->inode = inode_count++;
    root->parent = NULL;
    root->child_count = 0;
    root->data = NULL;
    root->size = 0;
    root->capacity = 0;

    return ramfs_to_vfs(root);
}
