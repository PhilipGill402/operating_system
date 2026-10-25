#include <arch/memory/memory_map.h>
#include "memory/memory_layout.h"

extern uint8_t kernel_end[];

const uintptr_t arch_user_virt_start = USER_VIRT_START;
const uintptr_t arch_user_virt_end = USER_VIRT_END;

const uintptr_t arch_kernel_virt_start = KERNEL_VIRT_START;
const uintptr_t arch_kernel_virt_end = KERNEL_VIRT_END;

const uintptr_t arch_kernel_start = KERNEL_BASE;
const size_t arch_kernel_size = KERNEL_REGION_SIZE;
const uintptr_t arch_kernel_end = (uintptr_t)kernel_end;
const uintptr_t arch_kernel_offset = KERNEL_OFFSET;

const uintptr_t arch_kheap_start = KHEAP_START;
const size_t arch_kheap_size = KHEAP_SIZE;
const size_t arch_kheap_initial_size = KHEAP_INITIAL_SIZE;
const uintptr_t arch_kheap_end = KHEAP_END;

const uintptr_t arch_temp_map_start = TEMP_MAP_BASE;
const uint8_t arch_temp_num_regions = NUM_TEMP_REGIONS;
const size_t arch_temp_map_size = TEMP_MAP_SIZE;
const uintptr_t arch_temp_map_end = TEMP_MAP_END;

const uintptr_t arch_phys_map_start = PHYS_MAP_BASE;
const size_t arch_phys_map_size = PHYS_MAP_SIZE;
const uintptr_t arch_phys_map_end = PHYS_MAP_END;

const uintptr_t arch_kstack_region_start = KSTACK_REGION_START;
const size_t arch_kstack_region_size = KSTACK_REGION_SIZE;
const size_t arch_kstack_size = KSTACK_SIZE;
const uintptr_t arch_kstack_region_end = KSTACK_REGION_END;

const uintptr_t arch_user_image_start = USER_IMAGE_START;
const uintptr_t arch_user_heap_start = USER_HEAP_START;
const uintptr_t arch_user_heap_end = USER_HEAP_END;
const uintptr_t arch_user_stack_start = USER_STACK_BOTTOM;
const uintptr_t arch_user_stack_end = USER_STACK_TOP;
