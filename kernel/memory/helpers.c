#include "memory/helpers.h"
#include <arch/memory/memory_map.h>
#include <helpers.h>
#include <string.h>

#include <log.h>

void* phys_to_virt(uintptr_t phys) {
    return (void*)(phys + arch_kernel_offset);
}

uint8_t memory_copy_to_user(void* user_dst, void* kernel_src, size_t size, arch_address_space_t* addr_space) {
    size_t remaining_size = size;
    uintptr_t addr = (uintptr_t)user_dst;
    while (remaining_size > 0) {
        uintptr_t phys = arch_virt_to_phys(addr_space, addr);
        if (!phys)
            return 0;

        uintptr_t frame = ALIGN_DOWN(phys, PAGE_SIZE);
        uintptr_t offset = phys & (PAGE_SIZE - 1);
        uint8_t* page = (uint8_t*)arch_phys_temp_map(frame);
        if (!page)
            return 0;

        size_t bytes_to_copy = remaining_size < (PAGE_SIZE - offset) ? remaining_size : (PAGE_SIZE - offset);
        memcpy(page + offset, kernel_src, bytes_to_copy);

        arch_phys_temp_unmap(page);
        addr += bytes_to_copy;
        remaining_size -= bytes_to_copy;
        kernel_src = (uint8_t*)kernel_src + bytes_to_copy;
    }

    return 1;
}

uint8_t memory_copy_from_user(void* kernel_dst, void* user_src, size_t size, arch_address_space_t* addr_space) {
    size_t remaining_size = size;
    uintptr_t addr = (uintptr_t)user_src;
    while (remaining_size > 0) {
        uintptr_t phys = arch_virt_to_phys(addr_space, addr);
        if (!phys)
            return 0;

        uintptr_t frame = ALIGN_DOWN(phys, PAGE_SIZE);
        uintptr_t offset = phys & (PAGE_SIZE - 1);
        
        uint8_t* page = (uint8_t*)arch_phys_temp_map(frame);
        if (!page)
            return 0;

        size_t bytes_to_copy = remaining_size < (PAGE_SIZE - offset) ? remaining_size : (PAGE_SIZE - offset);
        memcpy(kernel_dst, page + offset, bytes_to_copy);
        
        arch_phys_temp_unmap(page);
        addr += bytes_to_copy;
        remaining_size -= bytes_to_copy;
        kernel_dst = (uint8_t*)kernel_dst + bytes_to_copy;
    } 

    return 1;
}
