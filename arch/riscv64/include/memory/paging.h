#ifndef ARCH_RISCV64_INCLUDE_MEMORY_PAGING_H_
#define ARCH_RISCV64_INCLUDE_MEMORY_PAGING_H_

#include <stdint.h>

#define VPN0(va) (((va) >> 12) & 0x1FF)
#define VPN1(va) (((va) >> 21) & 0x1FF)
#define VPN2(va) (((va) >> 30) & 0x1FF)

#define PTE_V           (1ULL << 0)
#define PTE_R           (1ULL << 1)
#define PTE_W           (1ULL << 2)
#define PTE_X           (1ULL << 3)
#define PTE_U           (1ULL << 4)
#define PTE_G           (1ULL << 5)
#define PTE_A           (1ULL << 6)
#define PTE_D           (1ULL << 7)
#define PTE_OWNED       (1ULL << 8)
#define PTE_PHYS(pte)   ((pte >> 10) << 12)

#define TEMP_MAP_BASE  0xFFFFFFC000000000ULL

typedef struct arch_address_space {
    uint64_t root_pt;
} arch_address_space_t;

uint8_t riscv_paging_init();

extern arch_address_space_t kernel_address_space;

#define SATP_SV39 (8ULL << 60)

#endif
