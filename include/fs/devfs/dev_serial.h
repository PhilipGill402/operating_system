#ifndef INCLUDE_FS_DEVFS_DEV_SERIAL_H_
#define INCLUDE_FS_DEVFS_DEV_SERIAL_H_

#include <stdint.h>

#include "fs/fs_types.h"
#include "fs/devfs/dev_defs.h"
#include "memory/heap.h"
#include "io/serial.h"

dev_file_t* create_serial_file(fs_node_t* parent, uint32_t inode);

#endif // !INCLUDE_FS_DEVFS_DEV_SERIAL_H_
