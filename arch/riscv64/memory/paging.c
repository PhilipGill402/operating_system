#include <arch/memory/paging.h>
#include "memory/paging.h"
#include "memory/physical_allocator.h"
#include "memory/temp_map_allocator.h"
#include "memory/memory_layout.h"
#include "memory/mmio.h"
#include <string.h>
#include <helpers.h>

#include <log.h>

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

arch_address_space_t kernel_address_space = { 0 };

static inline void* phys_to_virt(uintptr_t phys) {
    return (void*)(phys + arch_kernel_offset);
}

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

    memset(phys_to_virt(page_table), 0, PAGE_SIZE);

    return page_table;
}

static uint64_t* riscv_walk_page_table(uintptr_t root_pt, uintptr_t virt_addr, uint8_t create) {
    uint64_t* root = (uint64_t*)phys_to_virt((uintptr_t)root_pt);
    uint16_t vpn2 = VPN2(virt_addr);
    uint16_t vpn1 = VPN1(virt_addr);
    uint16_t vpn0 = VPN0(virt_addr);

    uint64_t pte2 = root[vpn2];
    
    // if the pte is not valid, check if we should create a new pte or fail
    if (!pte_valid(pte2)) {
        if (!create)
            return 0;

        uintptr_t pte_phys = riscv_alloc_page_table();
        if (!pte_phys)
            return 0;

        pte2 = ((pte_phys >> 12) << 10) | PTE_V;
        root[vpn2] = pte2;
    }
    
    if (pte_leaf(pte2))
        return 0;
    
    uint64_t* pt1 = (uint64_t*)phys_to_virt(PTE_PHYS(pte2));
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

    uint64_t* pt0 = (uint64_t*)phys_to_virt(PTE_PHYS(pte1));

    return &pt0[vpn0];
}

static uint8_t riscv_temp_map_init() {
    for (uint8_t slot = 0; slot < arch_temp_num_regions; slot++) {
        uintptr_t virt = arch_temp_map_start + slot * PAGE_SIZE;
        uint64_t* pte = riscv_walk_page_table(kernel_address_space.root_pt, virt, 1);

        if (!pte)
            return 0;
    
        if (pte_valid(*pte)) {
            log_error("temp map region already mapped\n");
            return 0;
        }

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

        uint64_t flags = ARCH_PAGE_READ | ARCH_PAGE_WRITE;
        if (i == MMIO_RAM_IDX) {
            for (uintptr_t phys = start; phys < end; phys += PAGE_SIZE) {
                uintptr_t virt = phys + arch_kernel_offset;
                flags |= ARCH_PAGE_EXECUTE;

                if (!arch_page_map(&kernel_address_space, virt, phys, flags))
                    return 0;
            }

            continue;
        }

        for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
            if (!arch_page_map(&kernel_address_space, addr + KERNEL_MMIO_BASE, addr, flags)) {
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
    uint64_t* pte = riscv_walk_page_table(space->root_pt, virt, 1);
    if (!pte)
        return 0;
    
    if (pte_valid(*pte))
        return 0;

    // set pte to physical address and flags
    uint64_t riscv_flags = kernel_flags_to_riscv_flags(flags) | PTE_A;
    if (riscv_flags & PTE_W)
        riscv_flags |= PTE_D;

    *pte = ((phys >> 12) << 10) | riscv_flags | PTE_V;

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

uintptr_t arch_virt_to_phys(arch_address_space_t* addr_space, uintptr_t virt) {
    uint64_t* pte = riscv_walk_page_table(addr_space->root_pt, virt, 0);
    
    if (!pte || !pte_valid(*pte))
        return 0;

    uintptr_t page_phys = PTE_PHYS(*pte);
    uintptr_t offset = virt & (PAGE_SIZE - 1);

    return page_phys + offset;
}

void* arch_phys_temp_map(uintptr_t phys) {
    uint8_t slot = temp_slot_alloc();
    if (slot >= arch_temp_num_regions)
        return 0;
    
    uintptr_t virt = arch_temp_map_start + slot * PAGE_SIZE;

    // get pte
    uint64_t* pte = riscv_walk_page_table(kernel_address_space.root_pt, virt, 1);
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

    return (void*)virt;
}

void arch_phys_temp_unmap(void* virt) {
    uint8_t slot = ((uintptr_t)virt - arch_temp_map_start) / PAGE_SIZE;
    if (slot >= arch_temp_num_regions)
        return;
    
    uint64_t* pte = riscv_walk_page_table(kernel_address_space.root_pt, (uintptr_t)virt, 0);
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

uintptr_t arch_address_space_root(const arch_address_space_t* space) {
    return space->root_pt;
}

void arch_debug_mapping(arch_address_space_t* space, uintptr_t virt) {
    uint64_t* pte = riscv_walk_page_table(space->root_pt, virt, 0);

    if (!pte) {
        log_debug("VA %lx: no PTE\n", virt);
        return;
    }

    log_debug("VA  %lx\n", virt);
    log_debug("PTE %lx\n", *pte);
    log_debug("PA  %lx\n", PTE_PHYS(*pte));
}
