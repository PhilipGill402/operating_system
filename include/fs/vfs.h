#ifndef INCLUDE_FS_VFS_H_
#define INCLUDE_FS_VFS_H_

#include <stdint.h>
#include <string.h>
#include "fs/fs_types.h"
#include "memory/heap.h"

#define MAX_MOUNTS 16

fs_node_t* vfs_init();
int fs_mount(fs_node_t* root, char* name);

#endif // !INCLUDE_FS_VFS_H_

