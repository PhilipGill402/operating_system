#ifndef INCLUDE_FS_INITRD_H_
#define INCLUDE_FS_INITRD_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "fs/fs.h"
#include "memory/heap.h"

typedef struct {
    uint32_t num_files;
} __attribute__((packed)) initrd_header_t;

typedef struct {
    char name[64];
    uint32_t offset;
    uint32_t size;
} __attribute__((packed)) initrd_file_header_t;

fs_node_t* initrd_init(uint32_t addr);

#endif
