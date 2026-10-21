#include "memory/helpers.h"
#include <helpers.h>
#include <string.h>

uint8_t memory_copy_to_user(void* dst, void* src, size_t size, arch_address_space_t* addr_space) {
    uintptr_t phys_addr = arch_virt_to_phys(addr_space, (uintptr_t)dst);
    if (!phys_addr)
        return 0;
    
    uintptr_t frame = ALIGN_DOWN(phys_addr, PAGE_SIZE);
    uintptr_t offset = phys_addr & (PAGE_SIZE - 1);
    if (offset + size > PAGE_SIZE)
        return 0;

    uint8_t* page = (uint8_t*)arch_phys_temp_map(frame);
    if (!page)
        return 0;

    memcpy(page + offset, src, size);

    arch_phys_temp_unmap((uintptr_t)page);

    return 1;
}
