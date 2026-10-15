#include <arch/memory/paging.h>
#include "memory/physical_allocator.h"
#include <string.h>

arch_address_space_t kernel_address_space = { 0 };

static inline uint8_t pte_valid(uint64_t pte) {
    return (pte & PTE_V) != 0;
}

static inline uint8_t pte_leaf(uint64_t pte) {
    return (pte & (PTE_R | PTE_W | PTE_X)) != 0;
}

static uintptr_t riscv_alloc_page_table() {
    uintptr_t page_table = pmm_alloc_frame();
    if (!page_table)
        return 0;
    
    memset((void*)page_table, 0, PAGE_SIZE);

    return page_table;
}

static uint64_t* riscv_walk_page_table(uint64_t* root_pt, uintptr_t virt_addr, uint8_t create) {
    uint16_t vpn2 = VPN2(virt_addr);
    uint16_t vpn1 = VPN1(virt_addr);
    uint16_t vpn0 = VPN0(virt_addr);

    uint64_t pte2 = root_pt[vpn2];
    
    // if the pte is not valid, check if we should create a new pte or fail
    if (!pte_valid(pte2)) {
        if (!create)
            return 0;

        uintptr_t pte_phys = riscv_alloc_page_table();
        if (!pte_phys)
            return 0;

        pte2 = ((pte_phys >> 12) << 10) | PTE_V;
        root_pt[vpn2] = pte2;
    }
    
    if (pte_leaf(pte2))
        return 0;
    
    uint64_t* pt1 = (uint64_t*)PTE_PHYS(pte2);
    uint64_t pte1 = pt1[vpn1];

    // if the pte is not valid, check if we should create a new pte or fail
    if (!pte_valid(pte1)) {
        if (!create)
            return 0;

        uintptr_t pte_phys = riscv_alloc_page_table();
        if (!pte_phys)
            return 0;

        pte1 = ((pte_phys >> 12) << 10) | PTE_V;
        pt1[vpn1] = pte1;
    }
    
    if (pte_leaf(pte1))
        return 0;

    uint64_t* pt0 = (uint64_t*)PTE_PHYS(pte1);

    return &pt0[vpn0];
}

uint8_t riscv_paging_init() {
    kernel_address_space = { .root_pt = riscv_alloc_page_table() };
    if (!kernel_address_space.root_pt)
        return 0;
}

uint8_t arch_paging_transition(void) {
    return 0;
}

uint8_t arch_paging_init_shared_region(void) {
    return 0;
}

uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uintptr_t phys, uint64_t flags) {
    // check alignment 
    if (virt_addr & (PAGE_SIZE - 1))
        return 0;

    if (phys_addr & (PAGE_SIZE - 1))
        return 0;

    // get pte
    uint64_t* pte = riscv_walk_page_table(space->root_pt, virt, 1);
    if (!pte)
        return 0;

    if (!pte_valid(pte))
        return 0;
    
    // set pte to physical address and flags
    *pte = ((phys_addr >> 12) << 10) | flags | PTE_V;

    return 1;
}

uintptr_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt) {
    uint64_t* pte = riscv_walk_page_table(space->root_pt, virt, 0);
    if (!pte)
        return 0;

    if (!pte_valid(*pte))
        return 0;
    
    uintptr_t phys = PTE_PHYS(*pte);
    *pte = 0;
    
    // flush virtual address from tlb
    asm volatile("sfence.vma %0, zero" : : "r"(virt) : "memory");
    
    return phys;
}

void* arch_phys_temp_map(uint32_t tmp_slot, uint32_t phys) {
    return NULL;
}

void arch_phys_temp_unmap(uint32_t slot) {
    return;
}
