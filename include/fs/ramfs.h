#ifndef INCLUDE_FS_RAMFS_H_
#define INCLUDE_FS_RAMFS_H_

#include <stdint.h>
#include "fs/fs_types.h"
#include "memory/heap.h"

#define RAMFS_NAME_MAX 128
#define RAMFS_CHILDREN_MAX 64
#define RAMFS_SIZE_MAX 256


typedef struct ramfs_node {
    fs_node_t* fs_node;

    char name[RAMFS_NAME_MAX];
    fs_node_type_t type;
    uint32_t inode;

    struct ramfs_node* parent;
    struct ramfs_node* children[RAMFS_CHILDREN_MAX];
    uint32_t child_count;

    uint8_t* data;
    uint32_t size;
    uint32_t capacity;
} ramfs_node_t;

fs_node_t* ramfs_init();

#endif // !INCLUDE_FS_RAMFS_H_
