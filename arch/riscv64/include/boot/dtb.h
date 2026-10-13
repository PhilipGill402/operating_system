#ifndef ARCH_RISCV64_INCLUDE_BOOT_DTB_H_
#define ARCH_RISCV64_INCLUDE_BOOT_DTB_H_

#include <stdint.h>
#include <stddef.h>

#define DTB_MAGIC           0xD00DFEED
#define DTB_MAX_NODE_NAME   256
#define DTB_STACK_SIZE      16

#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE   0x2
#define FDT_PROP       0x3
#define FDT_NOP        0x4
#define FDT_END        0x9

typedef struct __attribute__((packed)) dtb_tree {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} dtb_tree_t;

typedef struct dtb_struct_node {
    char* name;
    uint32_t address_cells;
    uint32_t size_cells;
    uint8_t is_memory;
} dtb_struct_node_t;

typedef struct dtb_token_stack {
    dtb_struct_node_t stack[DTB_STACK_SIZE];
    size_t size;
} dtb_token_stack_t;

uint8_t dtb_init(dtb_tree_t* dtb);
uint8_t dtb_parse_struct(dtb_tree_t* dtb);

#endif
