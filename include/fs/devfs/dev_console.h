#ifndef INCLUDE_FS_DEVFS_DEV_CONSOLE_H_
#define INCLUDE_FS_DEVFS_DEV_CONSOLE_H_

#include <stdint.h>

#include "fs/fs_types.h"
#include "fs/devfs/dev_defs.h"
#include "memory/heap.h"
#include "io/io.h"
#include "interrupts/keyboard.h"

dev_file_t* create_console_file(fs_node_t* parent, uint32_t inode);

#endif // !INCLUDE_DEVFS_DEV_CONSOLE_H_
