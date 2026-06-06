#ifndef INCLUDE_FS_PROC_PROC_H_
#define INCLUDE_FS_PROC_PROC_H_

#include <stdint.h>
#include <stdlib.h>

#include "fs/procfs/procfs_defs.h"
#include "memory/heap.h"

proc_file_t* create_proc_file(fs_node_t* parent, uint32_t pid, uint32_t inode);

#endif
