#include <arch/memory/paging.h>
#include <string.h>

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


