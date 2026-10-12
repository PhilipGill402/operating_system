#ifndef ARCH_RISCV64_INCLUDE_MEMORY_PAGING_H_
#define ARCH_RISCV64_INCLUDE_MEMORY_PAGING_H_

#define VPN0(va) (((va) >> 12) & 0x1FF)
#define VPN1(va) (((va) >> 21) & 0x1FF)
#define VPN2(va) (((va) >> 30) & 0x1FF)

#define PTE_V (1ULL << 0)
#define PTE_R (1ULL << 1)
#define PTE_W (1ULL << 2)
#define PTE_X (1ULL << 3)
#define PTE_U (1ULL << 4)
#define PTE_G (1ULL << 5)
#define PTE_A (1ULL << 6)
#define PTE_D (1ULL << 7)

#endif
