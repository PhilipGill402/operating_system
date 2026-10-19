#ifndef ARCH_RISCV64_INCLUDE_MEMORY_MMIO_H_
#define ARCH_RISCV64_INCLUDE_MEMORY_MMIO_H_

#include <stdint.h>
#include <stddef.h>

typedef struct mem_range {
    uintptr_t addr;
    size_t size;
} mem_range_t;


typedef enum mmio_region {
    MMIO_RAM_IDX = 0,
    MMIO_UART0_IDX,
    MMIO_PLIC_IDX,
    MMIO_CLINT_IDX,
    MMIO_NUM_REGIONS
} mmio_region_t;

#define RAM_BASE     (riscv_mmio_info[MMIO_RAM_IDX].addr)
#define RAM_SIZE     (riscv_mmio_info[MMIO_RAM_IDX].size)

#define UART0_BASE   (riscv_mmio_info[MMIO_UART0_IDX].addr)
#define UART0_SIZE   (riscv_mmio_info[MMIO_UART0_IDX].size)

#define PLIC_BASE    (riscv_mmio_info[MMIO_PLIC_IDX].addr)
#define PLIC_SIZE    (riscv_mmio_info[MMIO_PLIC_IDX].size)

#define CLINT_BASE   0x02000000UL
#define CLINT_SIZE   0x00010000UL

extern mem_range_t riscv_mmio_info[5];

#endif
