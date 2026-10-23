#ifndef INCLUDE_MEMORY_HELPERS_H_
#define INCLUDE_MEMORY_HELPERS_H_

#include <stdint.h>
#include <stddef.h>
#include <arch/memory/paging.h>

void* phys_to_virt(uintptr_t phys);
uint8_t memory_copy_to_user(void* dst, void* src, size_t size, arch_address_space_t* addr_space);

#endif
