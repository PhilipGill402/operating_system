#ifndef INCLUDE_FS_INITRD_H_
#define INCLUDE_FS_INITRD_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "fs/fs.h"
#include "memory/heap.h"

#define INITRD_MAGIC 0x52445449
#define INITRD_NAME_MAX 64
#define INITRD_INVALID_NODE 0xFFFFFFFF

typedef enum {
    INITRD_NODE_FILE = 1,
    INITRD_NODE_DIR  = 2
} initrd_node_type_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t node_count;
    uint32_t root_node;
    uint32_t nodes_offset;
} __attribute__((packed)) initrd_superblock_t;

typedef struct {
    uint32_t id;
    initrd_node_type_t type;
    uint32_t parent_id;
    char name[INITRD_NAME_MAX];
    uint32_t size;
    uint32_t data_offset;
} __attribute__((packed)) initrd_node_t;

extern initrd_superblock_t* superblock;
extern initrd_node_t* node_table;
extern uint32_t num_nodes;

fs_node_t* initrd_init(uint32_t addr);

#endif
