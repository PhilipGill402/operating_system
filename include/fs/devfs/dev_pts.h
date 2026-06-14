#ifndef INCLUDE_FS_DEVFS_DEV_PTY_H_
#define INCLUDE_FS_DEVFS_DEV_PTY_H_

#include <stdint.h>

#include <queue.h>

#include "fs/fs_types.h"
#include "fs/devfs/dev_defs.h"
#include "memory/heap.h"
#include "exec/scheduler.h"

dev_file_t* create_pts_file(fs_node_t* parent, uint32_t inode);

#endif // !INCLUDE_FS_DEVFS_DEV_PTY_H_
