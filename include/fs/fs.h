#ifndef INCLUDE_FS_FS_H_
#define INCLUDE_FS_FS_H_

#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"
#include "memory/paging.h"

typedef enum {
    FS_FILE,
    FS_DIR
} fs_node_type_t;

typedef struct {
    char name[128];
    uint32_t inode;
} dirent_t;

typedef struct fs_node_t fs_node_t;

typedef struct fs_node_t {
    char name[128];
    fs_node_type_t flags;
    uint32_t inode;
    uint32_t size;

    uint32_t (*read)(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
    dirent_t (*readdir)(fs_node_t* node, uint32_t index);
    fs_node_t* (*finddir)(fs_node_t* node, char* name);
} fs_node_t;

uint8_t fs_init(multiboot_info_t* mbi, fs_node_t* (*init)(uint32_t addr));
uint32_t fs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
dirent_t fs_readdir(fs_node_t* node, uint32_t index);
fs_node_t* fs_finddir(fs_node_t* node, char* name);

extern fs_node_t* fs_root;

#endif // !INCLUDE_FS_FS_H_
