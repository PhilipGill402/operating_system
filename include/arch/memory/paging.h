#ifndef INCLUDE_ARCH_MEMORY_PAGING_H_
#define INCLUDE_ARCH_MEMORY_PAGING_H_

#include <stdint.h>
#include <stddef.h>

#include "memory_mapping.h"

typedef struct arch_address_space arch_address_space_t;

#define ARCH_PAGE_PRESENT   0x001
#define ARCH_PAGE_WRITE     0x010
#define ARCH_PAGE_USER      0x100

#define NUM_TEMP_REGIONS    8

// INIT FUNCTIONS
uint8_t arch_paging_transition(void);
uint8_t arch_paging_init_shared_region(void);

// KERNEL ADDRESS SPACE
arch_address_space_t* arch_kernel_address_space(void);

// ADDRESS SPACE MANAGEMENT
arch_address_space_t* arch_address_space_create(void);
arch_address_space_t* arch_address_space_clone(arch_address_space_t* old);
void arch_address_space_destroy(arch_address_space_t* address_space);
uint8_t arch_address_space_activate(arch_address_space_t* address_space);

// MEMORY PAGE OPERATIONS
uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uintptr_t phys, uint64_t flags);
uintptr_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt);
uintptr_t arch_phys_temp_map(uintptr_t phys);
void arch_phys_temp_unmap(uintptr_t virt);

#endif // !INCLUDE_ARCH_MEMORY_PAGING_H_
