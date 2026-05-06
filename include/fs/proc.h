#ifndef INCLUDE_FS_PROC_H_
#define INCLUDE_FS_PROC_H_

#include "exec/process.h"

typedef struct {
    fs_node_t* children[MAX_PROCESSES];
    uint32_t child_count;
    fs_node_t* parent;
} proc_dir_t;

extern fs_node_t* proc_dir;

void init_proc();

#endif // !INCLUDE_FS_PROC_H_
