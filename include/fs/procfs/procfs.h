#ifndef INCLUDE_FS_PROC_PROCFS_H_
#define INCLUDE_FS_PROC_PROCFS_H_

#include <stdint.h>

#include "exec/process.h"
#include "fs/fs_types.h"
#include "fs/procfs/procfs_defs.h"
#include "fs/procfs/proc.h"
#include "fs/procfs/proc_cpuinfo.h"

fs_node_t* init_proc();

#endif // !INCLUDE_FS_PROC_H_
