#ifndef INCLUDE_MEMORY_HELPERS_H_
#define INCLUDE_MEMORY_HELPERS_H_

#include <stdint.h>
#include <stddef.h>
#include <arch/memory/paging.h>

void* phys_to_virt(uintptr_t phys);
uint8_t memory_copy_to_user(void* user_dst, void* kernel_src, size_t size, arch_address_space_t* addr_space);
uint8_t memory_copy_from_user(void* kernel_dst, void* user_src, size_t size, arch_address_space_t* addr_space);

#endif
