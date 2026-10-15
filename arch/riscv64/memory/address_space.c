#include <arch/memory/paging.h>
#include "memory/paging.h"
#include "memory/address_space_allocator.h"
#include "memory/physical_allocator.h"
#include <string.h>

static inline uint8_t pte_valid(uint64_t pte) {
    return (pte & PTE_V) != 0;
}

static inline uint8_t pte_leaf(uint64_t pte) {
    return (pte & (PTE_R | PTE_W | PTE_X)) != 0;
}



static void riscv_page_table_free(uint64_t* root_pt) {
    for (uint16_t i = 0; i < 512; i++) {
        uint64_t pte2 = root_pt[i];

        if (!pte_valid(pte2))
            continue;

        if (pte_leaf(pte2)) {
            if (pte2 & PTE_OWNED)
                pmm_free_frame(PTE_PHYS(pte2));
            
            root_pt[i] = 0;        
            continue;
        }
        
        uintptr_t pt1_phys = PTE_PHYS(pte2);
        uint64_t* pt1 = (uint64_t*)pt1_phys;
        for (uint16_t j = 0; j < 512; j++) {
            uint64_t pte1 = pt1[j];

            if (!pte_valid(pte1))
                continue;

            if (pte_leaf(pte1)) {
                if (pte1 & PTE_OWNED)
                    pmm_free_frame(PTE_PHYS(pte1));
                
                pt1[j] = 0;
                continue;
            }
            
            uintptr_t pt0_phys = PTE_PHYS(pte1);
            uint64_t* pt0 = (uint64_t*)pt0_phys;
            for (uint16_t k = 0; k < 512; k++) {
                uint64_t pte0 = pt0[k];

                if (!pte_valid(pte0))
                    continue;

                if (pte_leaf(pte0)) {
                    if (pte0 & PTE_OWNED)
                        pmm_free_frame(PTE_PHYS(pte0));
                    
                    pt0[k] = 0;
                }

            }

            pmm_free_frame(pt0_phys);
            pt1[j] = 0;
        }
        
        pmm_free_frame(pt1_phys);
        root_pt[i] = 0;
    } 
}

static uint64_t* riscv_page_table_clone(uint64_t* src_pt) {
    uint64_t* dst_pt = (uint64_t*)pmm_alloc_frame();
    if (!dst_pt)
        return NULL;
    memset(dst_pt, 0, PAGE_SIZE);

    for (uint16_t i = 0; i < 512; i++) {
        uint64_t pte2 = src_pt[i];

        if (!pte_valid(pte2))
            continue;

        if (pte_leaf(pte2)) {
            if (pte2 & PTE_OWNED) {
                uintptr_t new_phys = pmm_alloc_frame();
                if (!new_phys)
                    goto clone_cleanup;

                uint64_t flags = pte2 & 0x3FF;
                uint64_t new_pte = ((new_phys >> 12) << 10) | flags;
                uintptr_t old_phys = PTE_PHYS(pte2);

                // TODO: this should copy 1 GiB 
                memcpy((void*)new_phys, (void*)old_phys, PAGE_SIZE);

                dst_pt[i] = new_pte;
            } else {
                dst_pt[i] = pte2;
            }
                
            continue;
        }
        
        uint64_t* dst_pt1 = (uint64_t*)pmm_alloc_frame();
        if (!dst_pt1)
            goto clone_cleanup;
        memset(dst_pt1, 0, PAGE_SIZE);
        dst_pt[i] = (((uintptr_t)dst_pt1 >> 12) << 10) | PTE_V;


        uintptr_t pt1_phys = PTE_PHYS(pte2);
        uint64_t* pt1 = (uint64_t*)pt1_phys;
        for (uint16_t j = 0; j < 512; j++) {
            uint64_t pte1 = pt1[j];

            if (!pte_valid(pte1))
                continue;

            if (pte_leaf(pte1)) {
                if (pte1 & PTE_OWNED) {
                    uintptr_t new_phys = pmm_alloc_frame();
                    if (!new_phys)
                        goto clone_cleanup;

                    uint64_t flags = pte1 & 0x3FF;
                    uint64_t new_pte = ((new_phys >> 12) << 10) | flags;
                    uintptr_t old_phys = PTE_PHYS(pte1);

                    // TODO: this should copy 2MiB pages 
                    memcpy((void*)new_phys, (void*)old_phys, PAGE_SIZE);

                    dst_pt1[j] = new_pte;
                } else {
                    dst_pt1[j] = pte1;
                }

                continue;
            }

            uint64_t* dst_pt0 = (uint64_t*)pmm_alloc_frame();
            if (!dst_pt0)
                goto clone_cleanup;
            memset(dst_pt0, 0, PAGE_SIZE);
            dst_pt1[j] = (((uintptr_t)dst_pt0 >> 12) << 10) | PTE_V;
            
            uintptr_t pt0_phys = PTE_PHYS(pte1);
            uint64_t* pt0 = (uint64_t*)pt0_phys;
            for (uint16_t k = 0; k < 512; k++) {
                uint64_t pte0 = pt0[k];

                if (!pte_valid(pte0))
                    continue;

                if (pte_leaf(pte0)) {
                    if (pte0 & PTE_OWNED) {
                        uintptr_t new_phys = pmm_alloc_frame();
                        if (!new_phys)
                            goto clone_cleanup;

                        uint64_t flags = pte0 & 0x3FF;
                        uint64_t new_pte = ((new_phys >> 12) << 10) | flags;
                        uintptr_t old_phys = PTE_PHYS(pte0);

                        memcpy((void*)new_phys, (void*)old_phys, PAGE_SIZE);

                        dst_pt0[k] = new_pte;
                    } else {
                        dst_pt0[k] = pte0;
                    }

                    continue;
                }
            }
        }
    }

    return dst_pt;

clone_cleanup:
    riscv_page_table_free((uint64_t*)dst_pt);
    pmm_free_frame((uintptr_t)dst_pt);
    return NULL;
}

arch_address_space_t* arch_kernel_address_space(void) {
    return &kernel_address_space;
}

arch_address_space_t* arch_address_space_create(void) {
    arch_address_space_t* addr_space = address_space_alloc();
    if (!addr_space)
        return NULL;

    addr_space->root_pt = pmm_alloc_frame();
    if (!addr_space->root_pt) {
        address_space_free(addr_space);
        return NULL;
    }
    
    memset((void*)addr_space->root_pt, 0, PAGE_SIZE);

    return addr_space;
}

arch_address_space_t* arch_address_space_clone(arch_address_space_t* old) {
    arch_address_space_t* addr_space = address_space_alloc();
    if (!addr_space)
        return NULL;

    riscv_page_table_clone((uint64_t*)old->root_pt);

    return NULL;
}

void arch_address_space_destroy(arch_address_space_t* address_space) {
    if (!address_space)
        return;

    riscv_page_table_free((uint64_t*)address_space->root_pt);
    pmm_free_frame(address_space->root_pt);
    address_space_free(address_space);
}

uint8_t arch_address_space_activate(arch_address_space_t* address_space) {
    uint64_t satp = SATP_SV39 | (address_space->root_pt >> 12);

    asm volatile("csrw satp, %0" : : "r"(satp) : "memory");
    asm volatile("sfence.vma" ::: "memory");

    return 1;
}


