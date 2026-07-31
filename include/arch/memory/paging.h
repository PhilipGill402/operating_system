#ifndef INCLUDE_ARCH_MEMORY_PAGING_H_
#define INCLUDE_ARCH_MEMORY_PAGING_H_

#include <stdint.h>
#include <stddef.h>

#include "memory_mapping.h"

typedef struct arch_address_space arch_address_space_t;

#define ARCH_PAGE_PRESENT   1
#define ARCH_PAGE_WRITE     2
#define ARCH_PAGE_USER      4

extern uint32_t boot_page_directory[1024];
extern uint32_t boot_page_table1[1024];

// INIT FUNCTIONS
uint8_t arch_paging_transition(void);
uint8_t arch_paging_init_shared_region(void);

// KERNEL ADDRESS SPACE
arch_address_space_t* arch_kernel_address_space(void);

// ADDRESS SPACE MANAGEMENT
uint8_t arch_address_space_create(arch_address_space_t* address_space);
void arch_address_space_destroy(arch_address_space_t* address_space);
uint8_t arch_address_space_activate(arch_address_space_t* address_space);

// MEMORY PAGE OPERATIONS
uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uint32_t phys, uint32_t flags);
uint32_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt);
void* arch_phys_temp_map(uint32_t tmp_slot, uint32_t phys);
void arch_phys_temp_unmap(uint32_t slot);

#endif // !INCLUDE_ARCH_MEMORY_PAGING_H_
