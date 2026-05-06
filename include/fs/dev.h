#ifndef INCLUDE_FS_DEV_H_
#define INCLUDE_FS_DEV_H_

#include <stdint.h>
#include <log.h>
#include "memory/heap.h"
#include "fs/fs_types.h"
#include "fs/console.h"

typedef struct {
    fs_node_t* children[16];
    uint32_t child_count;
    fs_node_t* parent;
} dev_dir_t;

extern fs_node_t* dev_dir;

void init_dev();

#endif
