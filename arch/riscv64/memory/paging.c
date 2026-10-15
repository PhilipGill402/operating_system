#include <arch/memory/paging.h>
#include "memory/paging.h"
#include "memory/physical_allocator.h"
#include <string.h>

arch_address_space_t kernel_address_space = { 0 };

static uint64_t* temp_pts[NUM_TEMP_REGIONS] = { 0 };

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

static uint8_t riscv_init_temp_pt(uint8_t slot) {
    uintptr_t pt_phys = riscv_alloc_page_table();
    if (!pt_phys)
        return 0;

    temp_pts[slot] = (uint64_t*)pt_phys;

    return 1;
}

uint8_t riscv_paging_init() {
    kernel_address_space.root_pt = riscv_alloc_page_table();
    if (!kernel_address_space.root_pt)
        return 0;

    for (uint8_t i = 0; i < NUM_TEMP_REGIONS; ++i) {
        if (!riscv_init_temp_pt(i))
            return 0;
    }
    
    arch_address_space_activate(&kernel_address_space);

    return 1;


}

uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uintptr_t phys, uint64_t flags) {
    // check alignment 
    if (virt & (PAGE_SIZE - 1))
        return 0;

    if (phys & (PAGE_SIZE - 1))
        return 0;

    // get pte
    uint64_t* pte = riscv_walk_page_table((uint64_t*)space->root_pt, virt, 1);
    if (!pte)
        return 0;

    if (!pte_valid(*pte))
        return 0;
    
    // set pte to physical address and flags
    *pte = ((phys >> 12) << 10) | flags | PTE_V;

    return 1;
}

uintptr_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt) {
    uint64_t* pte = riscv_walk_page_table((uint64_t*)space->root_pt, virt, 0);
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

uintptr_t arch_phys_temp_map(uint8_t slot, uintptr_t phys) {
    if (slot >= NUM_TEMP_REGIONS)
        return 0;

    // TODO: somehow obtain a virtual address
    uintptr_t virt = 0;

    // check alignment 
    if (virt & (PAGE_SIZE - 1))
        return 0;

    if (phys & (PAGE_SIZE - 1))
        return 0;

    // get pte
    uint64_t* pte = riscv_walk_page_table(temp_pts[slot], virt, 1);
    if (!pte)
        return 0;

    if (!pte_valid(*pte))
        return 0;
    
    // set pte to physical address and flags
    *pte = ((phys >> 12) << 10) | PTE_R | PTE_W | PTE_V;

    return virt;
}

void arch_phys_temp_unmap(uint8_t slot) {
    if (slot >= NUM_TEMP_REGIONS)
        return;
    
    // TODO: figure out how to calculate the virtual address
    uintptr_t virt = 0;

    // TODO: check whether this unmaps the whole page table or just the virtual address 
    uint64_t* pte = riscv_walk_page_table(temp_pts[slot], virt, 0);
    if (!pte)
        return;

    if (!pte_valid(*pte))
        return;
    
    uintptr_t phys = PTE_PHYS(*pte);
    *pte = 0;
    
    // flush virtual address from tlb
    asm volatile("sfence.vma %0, zero" : : "r"(virt) : "memory");
}
