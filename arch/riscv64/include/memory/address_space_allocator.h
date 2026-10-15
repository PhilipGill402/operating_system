#ifndef ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_
#define ARCH_MEMORY_ADDRESS_SPACE_ALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>

typedef struct arch_address_space arch_address_space_t;

arch_address_space_t* address_space_alloc(void);
void address_space_free(arch_address_space_t* addr_space);

#endif
