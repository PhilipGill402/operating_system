#ifndef INCLUDE_FS_DEV_DEV_DEVFS_H_
#define INCLUDE_FS_DEV_DEV_DEVFS_H_

#include <stdint.h>
#include <stddef.h>

#include "fs/fs_types.h"

typedef struct dev_file {
    fs_node_t* parent;
    char name[64];
    uint32_t inode;
    int32_t (*get_data)(struct dev_file* file, uint8_t* buffer, uint32_t offset, uint32_t size);
    int32_t (*write_data)(struct dev_file* file, uint8_t* buffer, uint32_t offset, uint32_t size);
} dev_file_t;

typedef struct {
    char name[64]; 
    dev_file_t* children[16];
    uint32_t child_count;
    fs_node_t* parent;
} dev_dir_t;

#endif
