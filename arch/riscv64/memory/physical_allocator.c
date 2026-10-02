#include <arch/memory/physical_allocator.h>


void pmm_init(multiboot_info_t* mbi) {
    return;
}

uint32_t pmm_alloc_frame(void)  {
    return 0;
}

void pmm_free_frame(uint32_t phys_addr) {
    return;
}
