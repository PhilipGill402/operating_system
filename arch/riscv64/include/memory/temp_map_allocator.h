#ifndef ARCH_RISCV64_INCLUDE_TEMP_MAP_ALLOCATOR_H_
#define ARCH_RISCV64_INCLUDE_TEMP_MAP_ALLOCATOR_H_

#include <stdint.h>

uint8_t temp_slot_alloc();
void temp_slot_free(uint8_t slot);

#endif
