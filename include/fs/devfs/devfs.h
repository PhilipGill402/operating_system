#ifndef INCLUDE_FS_DEV_H_
#define INCLUDE_FS_DEV_H_

#include <stdint.h>
#include <log.h>
#include "memory/heap.h"
#include "fs/fs_types.h"
#include "fs/devfs/dev_defs.h"
#include "fs/devfs/dev_console.h"

fs_node_t* init_dev();

#endif
