#ifndef INCLUDE_ARCH_MEMORY_PAGING_DEFS_H_
#define INCLUDE_ARCH_MEMORY_PAGING_DEFS_H_

#include <stdint.h>

#define PAGE_SIZE 4096

#define I686_PAGE_PRESENT   1
#define I686_PAGE_WRITE     2
#define I686_PAGE_USER      4

#define I686_ADDR_MASK      0xFFFFF000

#define TEMP_PD_INDEX_0 769
#define TEMP_PD_INDEX_1 770
#define TEMP_PD_INDEX_2 771
#define TEMP_PD_INDEX_3 772
#define TEMP_PD_INDEX_4 773
#define TEMP_PD_INDEX_5 774

struct arch_address_space {
    uint32_t* directory_virt;
    uint32_t directory_phys;
};

#endif // !INCLUDE_ARCH_MEMORY_PAGING_DEFS_H_
