#ifndef ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_
#define ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>

#include "memory_mapping.h"

typedef uint64_t address_space_allocator_t[64];

void* get_address_space(void);
uint8_t release_address_space(void* virt);

#endif
