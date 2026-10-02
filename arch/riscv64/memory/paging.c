#include <arch/memory/paging.h>

uint8_t arch_paging_transition(void) {
    return 0;
}

uint8_t arch_paging_init_shared_region(void) {
    return 0;
}

arch_address_space_t* arch_kernel_address_space(void) {
    return NULL;
}

arch_address_space_t* arch_address_space_create(void) {
    return NULL;
}

arch_address_space_t* arch_address_space_clone(arch_address_space_t* old) {
    return NULL;
}

void arch_address_space_destroy(arch_address_space_t* address_space) {
    return;
}

uint8_t arch_address_space_activate(arch_address_space_t* address_space) {
    return 0;
}

uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uint32_t phys, uint32_t flags) {
    return 0;
}

uint32_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt) {
    return 0;
}

void* arch_phys_temp_map(uint32_t tmp_slot, uint32_t phys) {
    return NULL;
}

void arch_phys_temp_unmap(uint32_t slot) {
    return;
}
