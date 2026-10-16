#include "memory/temp_map_allocator.h"
#include <arch/memory/paging.h>

static uint8_t temp_bitmap = 0;

static inline void set_bit(uint8_t slot) {
    temp_bitmap |= (1 << slot); 
}

static inline void clear_bit(uint8_t slot) {
    temp_bitmap &= ~(1 << slot);
}

static uint8_t test_bit(uint8_t slot) {
    return (temp_bitmap & (1 << slot)) != 0;
}

uint8_t temp_slot_alloc() {
    for (uint8_t i = 0; i < NUM_TEMP_REGIONS; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return i;
        }
    }

    return NUM_TEMP_REGIONS;
}

void temp_slot_free(uint8_t slot) {
    if (slot < 8) 
        clear_bit(slot);
}

