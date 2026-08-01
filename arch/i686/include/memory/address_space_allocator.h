#ifndef ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_
#define ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>

#include <string.h>

#include <memory/paging_defs.h>

#include "memory_mapping.h"

typedef uint64_t address_space_allocator_t[64];
typedef struct arch_address_space arch_address_space_t;

void* get_address_space(void);
uint8_t release_address_space(void* virt);

arch_address_space_t* allocate_address_space_object(void);
void release_address_space_object(arch_address_space_t* addr_space);

#endif
