#ifndef INCLUDE_FS_PROCFS_PROC_MEMINFO_H_
#define INCLUDE_FS_PROCFS_PROC_MEMINFO_H_

#include <stdint.h>

#include <arch/memory/physical_allocator.h>

#include "fs/procfs/procfs_defs.h"

proc_file_t* create_meminfo_file(fs_node_t* parent, uint32_t inode);

#endif // !INCLUDE_FS_PROCFS_PROC_MEMINFO_H_
