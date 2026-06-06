#ifndef INCLUDE_PROCFS_PROCFS_DEFS_H_
#define INCLUDE_PROCFS_PROCFS_DEFS_H_

#include <stdint.h>
#include "fs/fs_types.h"
#include "exec/process.h"

typedef struct proc_file{
    fs_node_t* parent;
    char name[64];
    uint32_t inode;
    uint32_t pid;
    int32_t (*get_data)(struct proc_file* file, uint8_t* buffer, uint32_t offset, uint32_t size);
} proc_file_t;

typedef struct {
    proc_file_t* children[MAX_PROCESSES];
    uint32_t child_count;
    char name[64];
    fs_node_t* parent;
} proc_dir_t;

#endif // !INCLUDE_PROCFS_PROCFS_DEFS_H_
