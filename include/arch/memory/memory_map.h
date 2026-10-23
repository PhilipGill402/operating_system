#ifndef INCLUDE_ARCH_MEMORY_MEMORY_MAP_H_
#define INCLUDE_ARCH_MEMORY_MEMORY_MAP_H_

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000ULL

extern const uintptr_t arch_user_virt_start;
extern const uintptr_t arch_user_virt_end;

extern const uintptr_t arch_kernel_virt_start;
extern const uintptr_t arch_kernel_virt_end;

extern const uintptr_t arch_kernel_start;
extern const size_t arch_kernel_size;
extern const uintptr_t arch_kernel_end;
extern const uintptr_t arch_kernel_offset;

extern const uintptr_t arch_kheap_start;
extern const size_t arch_kheap_size;
extern const size_t arch_kheap_initial_size;
extern const uintptr_t arch_kheap_end;

extern const uintptr_t arch_temp_map_start;
extern const uint8_t arch_temp_num_regions;
extern const size_t arch_temp_map_size;
extern const uintptr_t arch_temp_map_end;

extern const uintptr_t arch_phys_map_start;
extern const size_t arch_phys_map_size;
extern const uintptr_t arch_phys_map_end;

extern const uintptr_t arch_kstack_region_start;
extern const size_t arch_kstack_region_size;
extern const size_t arch_kstack_size;
extern const uintptr_t arch_kstack_region_end;

extern const uintptr_t arch_user_image_start;
extern const uintptr_t arch_user_heap_start;
extern const uintptr_t arch_user_heap_end;
extern const uintptr_t arch_user_stack_start;
extern const uintptr_t arch_user_stack_end;

#endif
