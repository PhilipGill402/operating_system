#include <arch/memory/paging.h>
#include "memory/paging.h"
#include "memory/physical_allocator.h"
#include "memory/temp_map_allocator.h"
#include "memory/mmio.h"
#include <string.h>
#include <helpers.h>

#include <log.h>

arch_address_space_t kernel_address_space = { 0 };

static inline uint8_t pte_valid(uint64_t pte) {
    return (pte & PTE_V) != 0;
}

static inline uint8_t pte_leaf(uint64_t pte) {
    return (pte & (PTE_R | PTE_W | PTE_X)) != 0;
}

static uint64_t kernel_flags_to_riscv_flags(uint64_t flags) {
    uint64_t riscv_flags = 0; 

    if (flags & ARCH_PAGE_WRITE)
        riscv_flags |= PTE_W;

    if (flags & ARCH_PAGE_READ)
        riscv_flags |= PTE_R;

    if (flags & ARCH_PAGE_EXECUTE)
        riscv_flags |= PTE_X;

    if (flags & ARCH_PAGE_USER)
        riscv_flags |= PTE_U;

    return riscv_flags;
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

static uint8_t riscv_temp_map_init() {
    for (uint8_t slot = 0; slot < NUM_TEMP_REGIONS; slot++) {
        uintptr_t virt = TEMP_MAP_BASE + slot * PAGE_SIZE;
        uint64_t* pte = riscv_walk_page_table((uint64_t*)kernel_address_space.root_pt, virt, 1);

        if (!pte)
            return 0;

        *pte = 0;
    }
    
    return 1;
}

uint8_t riscv_paging_init() {
    kernel_address_space.root_pt = riscv_alloc_page_table();
    if (!kernel_address_space.root_pt)
        return 0;
    
    if (!riscv_temp_map_init())
        return 0;

    for (uint8_t i = 0; i < MMIO_NUM_REGIONS; ++i) {
        uint64_t start = ALIGN_DOWN(riscv_mmio_info[i].addr, PAGE_SIZE);
        uint64_t end = ALIGN_UP(riscv_mmio_info[i].addr + riscv_mmio_info[i].size, PAGE_SIZE);
        
        uint64_t flags = ARCH_PAGE_READ | ARCH_PAGE_WRITE | PTE_A | PTE_D;
        if (i == MMIO_RAM_IDX)
            flags |= ARCH_PAGE_EXECUTE;

        for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
            if (!arch_page_map(&kernel_address_space, addr, addr, flags)) {
                return 0;
            }
        }
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
    
    if (pte_valid(*pte))
        return 0;

    // set pte to physical address and flags
    uint64_t riscv_flags = kernel_flags_to_riscv_flags(flags);
    *pte = ((phys >> 12) << 10) | riscv_flags | PTE_V;

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

uintptr_t arch_phys_temp_map(uintptr_t phys) {
    uint8_t slot = temp_slot_alloc();
    if (slot >= NUM_TEMP_REGIONS)
        return 0;

    uintptr_t virt = TEMP_MAP_BASE + slot * PAGE_SIZE;

    // get pte
    uint64_t* pte = riscv_walk_page_table((uint64_t*)kernel_address_space.root_pt, virt, 1);
    if (!pte) {
        temp_slot_free(slot);
        return 0;
    }
    
    // slot already mapped
    if (pte_valid(*pte)) {
        temp_slot_free(slot);
        return 0;
    }
    
    // set pte to physical address and flags
    *pte = ((phys >> 12) << 10) | PTE_R | PTE_W | PTE_A | PTE_D | PTE_V;

    // flush virtual address from tlb
    asm volatile("sfence.vma %0, zero" : : "r"(virt) : "memory");

    return virt;
}

void arch_phys_temp_unmap(uintptr_t virt) {
    uint8_t slot = (virt - TEMP_MAP_BASE) / PAGE_SIZE;
    if (slot >= NUM_TEMP_REGIONS)
        return;
    
    uint64_t* pte = riscv_walk_page_table((uint64_t*)kernel_address_space.root_pt, virt, 0);
    if (!pte) {
        temp_slot_free(slot);
        return;
    }

    if (!pte_valid(*pte)) {
        temp_slot_free(slot);
        return;
    }
    
    *pte = 0;
    
    // flush virtual address from tlb
    asm volatile("sfence.vma %0, zero" : : "r"(virt) : "memory");

    temp_slot_free(slot);
}
