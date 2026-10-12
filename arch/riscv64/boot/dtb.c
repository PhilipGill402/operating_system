#include "boot/dtb.h"
#include <log.h>

static inline uint32_t be32_to_cpu(uint32_t x) {
    return __builtin_bswap32(x);
}

static inline uint8_t* align_to_4(uint8_t* ptr) {
    return (uint8_t*)(((uintptr_t)ptr + 3) & ~3ULL);
}

uint8_t dtb_init(dtb_tree_t* dtb) {
    // DTB is in big endian, we must convert to little endian
    dtb->magic = be32_to_cpu(dtb->magic);
    dtb->totalsize = be32_to_cpu(dtb->totalsize);
    dtb->off_dt_struct = be32_to_cpu(dtb->off_dt_struct);
    dtb->off_dt_strings = be32_to_cpu(dtb->off_dt_strings);
    dtb->off_mem_rsvmap = be32_to_cpu(dtb->off_mem_rsvmap);
    dtb->version = be32_to_cpu(dtb->version);
    dtb->last_comp_version = be32_to_cpu(dtb->last_comp_version);
    dtb->boot_cpuid_phys = be32_to_cpu(dtb->boot_cpuid_phys);
    dtb->size_dt_strings = be32_to_cpu(dtb->size_dt_strings);
    dtb->size_dt_struct = be32_to_cpu(dtb->size_dt_struct);

    if (dtb->magic != DTB_MAGIC)
        return 0;

    return 1;
}

void dtb_parse_struct(dtb_tree_t* dtb) {
    uint8_t* struct_ptr = (uint8_t*)dtb + dtb->off_dt_struct;

    while (1) {
        uint32_t token = be32_to_cpu(*(uint32_t*)struct_ptr);
        struct_ptr += 4;

        switch (token) {
            case FDT_BEGIN_NODE: {
                char node_name[256];
                size_t size = 0;
                while (*struct_ptr != 0) {
                    if (size >= sizeof(node_name) - 1)
                        break;

                    node_name[size++] = *((char*)struct_ptr);
                    struct_ptr++;
                }

                node_name[size] = '\0';
                struct_ptr++;
                struct_ptr = align_to_4(struct_ptr);
                    
                break;
            }
            case FDT_END_NODE: break;
            case FDT_PROP: break;
            case FDT_NOP: break;
            case FDT_END: return;
            default: log_error("received unknow DTB token\n"); return;
        }
    }
}
