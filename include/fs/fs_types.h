#ifndef INCLUDE_FS_FS_TYPES_H_
#define INCLUDE_FS_FS_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "memory/heap.h"

typedef enum {
    FS_FILE,
    FS_DIR,
    FS_DEV,
    FS_PROC
} fs_node_type_t;

typedef struct {
    char name[128];
    uint32_t inode;
} fs_dirent_t;

typedef struct fs_node_t fs_node_t;

typedef struct fs_node_t{
    char name[128];
    fs_node_type_t flags;
    uint32_t inode;
    uint32_t size;
    void* device;
    void* proc;

    fs_node_t* mount_parent;

    uint32_t (*read)(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
    fs_dirent_t* (*readdir)(fs_node_t* node, uint32_t index);
    fs_node_t* (*finddir)(fs_node_t* node, char* name);
    fs_node_t* (*parent)(fs_node_t* node);
    void (*createdir)(fs_node_t* node, char* name);
    void (*createfile)(fs_node_t* node, char* name, uint32_t size);
    uint32_t (*writefile)(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size);
} fs_node_t;

typedef struct {
    fs_node_t* node;
    uint32_t flags;
    uint32_t offset;
    uint8_t in_use;
} file_desc_t;

typedef struct {
    char name[128];
    fs_node_t* root;
} mount_t;

extern fs_node_t* fs_root;
extern fs_node_t* fs_cwd;
extern fs_node_t* console_node;
extern uint32_t inode_count;

fs_node_t* fs_node_clone(fs_node_t* node);

#endif // !INCLUDE_FS_FS_TYPES_H_

