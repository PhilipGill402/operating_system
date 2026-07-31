#ifndef INCLUDE_FS_FS_H_
#define INCLUDE_FS_FS_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <log.h>

#include <arch/memory/paging.h>

#include "multiboot.h"
#include "memory/heap.h"
#include "fs/fs_types.h"
#include "fs/devfs/devfs.h"
#include "fs/procfs/procfs.h"
#include "fs/ramfs.h"
#include "fs/vfs.h"
#include "fs/initrd.h"
#include "memory_mapping.h"

#define MAX_PATH_LENGTH 256
#define MAX_FDS 128

uint8_t fs_init(multiboot_info_t* mbi);
fs_node_t* resolve_path(const char* path, fs_node_t* cwd);
fs_node_t* resolve_path_from(fs_node_t* start, const char* path);
fs_node_t* fs_parent(fs_node_t* node);
void fs_createdir(fs_node_t* node, char* name);
void fs_createfile(fs_node_t* node, char* name, uint32_t size);
int32_t fs_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size);
int32_t fs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
fs_dirent_t* fs_readdir(fs_node_t* node, uint32_t index);
fs_node_t* fs_finddir(fs_node_t* node, char* name);
uint8_t fs_poll(fs_node_t* node, uint32_t offset);

#endif // !INCLUDE_FS_FS_H_
