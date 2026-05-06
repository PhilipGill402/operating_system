#ifndef INCLUDE_FS_FS_H_
#define INCLUDE_FS_FS_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <log.h>
#include "multiboot.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "fs/fs_types.h"
#include "fs/dev.h"

#define MAX_PATH_LENGTH 256
#define MAX_FDS 128

uint8_t fs_init(multiboot_info_t* mbi, fs_node_t* (*init)(uint32_t addr));
fs_node_t* resolve_path(const char* path);
fs_node_t* resolve_path_from(fs_node_t* start, const char* path);

extern fs_node_t* initrd_init(uint32_t addr);

#endif // !INCLUDE_FS_FS_H_
