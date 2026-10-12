#include <arch/memory/physical_allocator.h>
#include "kernel.h"

pmm_t pmm = { 0 };


void pmm_init(const kernel_boot_info_t* info) {
    return;
}

uintptr_t pmm_alloc_frame(void)  {
    return 0;
}

void pmm_free_frame(uint32_t phys_addr) {
    return;
}
