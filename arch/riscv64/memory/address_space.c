#include <arch/memory/paging.h>
#include "memory/paging.h"
#include "memory/address_space_allocator.h"
#include "memory/physical_allocator.h"
#include "memory/mmio.h"
#include <string.h>

#include <log.h>

static inline uint8_t pte_valid(uint64_t pte) {
    return (pte & PTE_V) != 0;
}

static inline uint8_t pte_leaf(uint64_t pte) {
    return (pte & (PTE_R | PTE_W | PTE_X)) != 0;
}

static inline void* phys_to_virt(uintptr_t phys) {
    return (void*)(phys + arch_kernel_offset);
}

static void riscv_page_table_free(uintptr_t root_pt) {
    uint64_t* root = phys_to_virt(root_pt);
    for (uint16_t i = 0; i < 256; i++) {
        uint64_t pte2 = root[i];

        if (!pte_valid(pte2))
            continue;

        if (pte_leaf(pte2)) {
            if (pte2 & PTE_OWNED)
                pmm_free_frame(PTE_PHYS(pte2));
            
            root[i] = 0;        
            continue;
        }
        
        uintptr_t pt1_phys = PTE_PHYS(pte2);
        uint64_t* pt1 = phys_to_virt(pt1_phys);
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
            uint64_t* pt0 = (uint64_t*)phys_to_virt(pt0_phys);
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
        root[i] = 0;
    }

    pmm_free_frame(root_pt);
}

static uintptr_t riscv_page_table_clone(uintptr_t src_pt) {
    uintptr_t dst_pt = pmm_alloc_frame();
    if (!dst_pt)
        return 0;
    uint64_t* dst = phys_to_virt(dst_pt);
    memset(dst, 0, PAGE_SIZE);
    
    uint64_t* src = phys_to_virt(src_pt);
    for (uint16_t i = 0; i < 256; i++) {
        uint64_t pte2 = src[i];

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
                memcpy(phys_to_virt(new_phys), phys_to_virt(old_phys), PAGE_SIZE);

                dst[i] = new_pte;
            } else {
                dst[i] = pte2;
            }
                
            continue;
        }
        
        uintptr_t dst_pt1 = pmm_alloc_frame();
        if (!dst_pt1)
            goto clone_cleanup;

        uint64_t* dst_pt1_virt = phys_to_virt(dst_pt1);
        memset(dst_pt1_virt, 0, PAGE_SIZE);
        dst[i] = (((uintptr_t)dst_pt1 >> 12) << 10) | PTE_V;

        uintptr_t pt1_phys = PTE_PHYS(pte2);
        uint64_t* pt1 = phys_to_virt(pt1_phys);
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
                    memcpy(phys_to_virt(new_phys), phys_to_virt(old_phys), PAGE_SIZE);

                    dst_pt1_virt[j] = new_pte;
                } else {
                    dst_pt1_virt[j] = pte1;
                }

                continue;
            }

            uintptr_t dst_pt0 = pmm_alloc_frame();
            if (!dst_pt0)
                goto clone_cleanup;
            uint64_t* dst_pt0_virt = phys_to_virt(dst_pt0);

            memset(dst_pt0_virt, 0, PAGE_SIZE);
            dst_pt1_virt[j] = (((uintptr_t)dst_pt0 >> 12) << 10) | PTE_V;
            
            uintptr_t pt0_phys = PTE_PHYS(pte1);
            uint64_t* pt0 = (uint64_t*)phys_to_virt(pt0_phys);
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

                        memcpy(phys_to_virt(new_phys), phys_to_virt(old_phys), PAGE_SIZE);

                        dst_pt0_virt[k] = new_pte;
                    } else {
                        dst_pt0_virt[k] = pte0;
                    }

                    continue;
                }
            }
        }
    }

    return dst_pt;

clone_cleanup:
    riscv_page_table_free(dst_pt);
    return 0;
}

static uint8_t riscv_install_kernel_mappings(arch_address_space_t* space) {
    arch_address_space_t* kernel_space = arch_kernel_address_space();
    uint64_t* dst = phys_to_virt(space->root_pt);
    uint64_t* kernel_root = phys_to_virt(kernel_space->root_pt);
    for (uint16_t i = 256; i < 512; ++i)
        dst[i] = kernel_root[i];

    return 1;
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
    
    memset(phys_to_virt(addr_space->root_pt), 0, PAGE_SIZE);
    
    if (!riscv_install_kernel_mappings(addr_space)) {
        pmm_free_frame(addr_space->root_pt); 
        address_space_free(addr_space);
        return NULL;
    }


    return addr_space;
}

arch_address_space_t* arch_address_space_clone(arch_address_space_t* old) {
    if (!old)
        return NULL;

    arch_address_space_t* addr_space = address_space_alloc();
    if (!addr_space)
        return NULL;

    uintptr_t new_pt = riscv_page_table_clone(old->root_pt);
    if (!new_pt) {
        address_space_free(addr_space);
        return NULL;
    }

    addr_space->root_pt = new_pt;

    if (!riscv_install_kernel_mappings(addr_space)) {
        riscv_page_table_free(addr_space->root_pt); 
        address_space_free(addr_space);
        return NULL;
    }

    return addr_space;
}

void arch_address_space_destroy(arch_address_space_t* address_space) {
    if (!address_space)
        return;

    riscv_page_table_free(address_space->root_pt);
    address_space_free(address_space);
}

uint8_t arch_address_space_activate(arch_address_space_t* address_space) {
    uint64_t satp = SATP_SV39 | (address_space->root_pt >> 12);

    asm volatile("csrw satp, %0" : : "r"(satp) : "memory");
    asm volatile("sfence.vma" ::: "memory");

    return 1;
}


