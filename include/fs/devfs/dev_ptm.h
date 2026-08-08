#ifndef INCLUDE_FS_DEVFS_DEV_PTM_H_
#define INCLUDE_FS_DEVFS_DEV_PTM_H_

#include <stdint.h>

#include <queue.h>
#include <errno.h>

#include "fs/fs_types.h"
#include "fs/devfs/dev_defs.h"
#include "memory/heap.h"
#include "exec/scheduler.h"

dev_file_t* create_ptm_file(fs_node_t* parent, uint32_t inode);

#endif // !INCLUDE_FS_DEVFS_DEV_PTM_H_
