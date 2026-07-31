#include <memory/address_space_allocator.h>

static address_space_allocator_t allocator = { 0 };

static int8_t get_index(uint32_t idx) {
    if (idx >= PAGE_DIRECTORY_MAP_SLOTS)
        return -1;

    uint32_t bit = idx % 64;
    uint32_t num = idx / 64;

    return (allocator[num] & 1ULL << bit) != 0; 
}

static void set_index(uint32_t idx) {
    if (idx >= PAGE_DIRECTORY_MAP_SLOTS)
        return;

    uint32_t bit = idx % 64;
    uint32_t num = idx / 64;

    allocator[num] |= 1ULL << bit;
}

static void reset_index(uint32_t idx) {
    if (idx >= PAGE_DIRECTORY_MAP_SLOTS)
        return;

    uint32_t bit = idx % 64;
    uint32_t num = idx / 64; 

    allocator[num] &= ~(1ULL << bit);
}

void* get_address_space(void) {
    for (uint32_t i = 0; i < PAGE_DIRECTORY_MAP_SLOTS; i++) {
        if (!get_index(i)) {
            set_index(i);
            return (void*)(PAGE_DIRECTORY_MAP_START + (i * PAGE_SIZE));
        }
    }

    return NULL;
}

uint8_t release_address_space(void* virt) { 
    if (virt == 0)
        return 0;

    uintptr_t addr = (uintptr_t)virt;

    if (addr < PAGE_DIRECTORY_MAP_START || addr >= PAGE_DIRECTORY_MAP_END)
        return 0;
    
    uintptr_t offset = addr - PAGE_DIRECTORY_MAP_START;
    if (offset % PAGE_SIZE != 0)
        return 0;
    
    uint32_t idx = (uint32_t)offset / PAGE_SIZE;
    reset_index(idx);

    return 1;
}

