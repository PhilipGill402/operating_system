#ifndef INCLUDE_FS_PROC_PROC_CPUINFO_H_
#define INCLUDE_FS_PROC_PROC_CPUINFO_H_

#include <stdint.h>

#include "fs/procfs/procfs_defs.h"
#include "memory/heap.h"

proc_file_t* create_cpuinfo_file(fs_node_t* parent, uint32_t inode);

#endif
