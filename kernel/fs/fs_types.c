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
    
    return clone;
}

