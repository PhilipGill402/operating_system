#ifndef INCLUDE_FS_PROC_H_
#define INCLUDE_FS_PROC_H_

#include "exec/process.h"
#include "fs/fs_types.h"

typedef struct {
    fs_node_t* children[MAX_PROCESSES];
    uint32_t child_count;
    fs_node_t* parent;
} proc_dir_t;

fs_node_t* init_proc();

#endif // !INCLUDE_FS_PROC_H_
