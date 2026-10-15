#include <memory/address_space_allocator.h>
#include <memory/address_space.h>
#include <string.h>

static arch_address_space_t address_space_pool[MAX_ADDRESS_SPACES];
static uint8_t address_space_used[MAX_ADDRESS_SPACES] = { 0 };

arch_address_space_t* address_space_alloc(void) {
    for (uint32_t i = 0; i < MAX_ADDRESS_SPACES; i++) {
        if (!address_space_used[i]) {
            address_space_used[i] = 1;
            memset(&address_space_pool[i], 0, sizeof(address_space_pool[i]));
            return &address_space_pool[i];
        }
    }

    return NULL;
}

void address_space_free(arch_address_space_t* addr_space) {
    // Pointer is not valid 
    if (!addr_space || addr_space < &address_space_pool[0] || addr_space >= &address_space_pool[MAX_ADDRESS_SPACES])
        return;

    uint32_t idx = (uint32_t)(addr_space - address_space_pool);
    
    memset(&address_space_pool[idx], 0, sizeof(*addr_space));
    address_space_used[idx] = 0;
}

