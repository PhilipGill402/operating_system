#include "fs/fs_types.h"

#include <log.h>

fs_node_t* fs_root;
fs_node_t* fs_cwd;
fs_node_t* console_node;
uint32_t inode_count = 0;

fs_node_t* fs_node_clone(fs_node_t* node) {
    if (!node) return NULL;

    fs_node_t* clone = kmalloc(sizeof(fs_node_t));
    if (!clone) return NULL;

    memcpy(clone, node, sizeof(fs_node_t));
    strcpy(clone->name, node->name);

    if (node->mount_parent)
        clone->mount_parent = fs_node_clone(node->mount_parent);
    
    return clone;
}

file_desc_t* fs_create_file_desc(fs_node_t* node, uint32_t flags) {
    file_desc_t* fd = kmalloc(sizeof(file_desc_t));
    if (!fd)
        return NULL;

    fd->node = fs_node_clone(node);
    if (!fd->node) {
        kfree(fd);
        return NULL;
    }

    fd->offset = 0;
    fd->flags = flags;
    fd->num_refs = 1;

    return fd;
}

void fs_free_file_desc(file_desc_t* fd) {
    if (!fd)
        return;
    
    if (!fd->node)
        return;

    fd->num_refs--;
    
    if (fd->num_refs == 0) {
        kfree(fd->node);
        kfree(fd);
    }
}
