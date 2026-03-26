#include "paging.h"

uint32_t* temp_page_table;
uint32_t* kernel_page_directory = (uint32_t*)&boot_page_directory;
uint32_t kernel_page_directory_phys = (uint32_t)&boot_page_directory - KERNEL_BASE;

void map_boot_page(uint32_t phys_addr) {
    uint32_t virt_addr = phys_addr + KERNEL_BASE;
    uint32_t pt_idx = (virt_addr >> 12) & 0x3FF;

    boot_page_table1[pt_idx] = (phys_addr & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;
    invlpg((void*)virt_addr);
}

void create_temp_page_table(uint32_t* virt_page_directory) {
    if (virt_page_directory[TEMP_PD_INDEX] & PAGE_PRESENT) {
        printf("Couldn't allocate temp page table\n");
        return;
    }

    uint32_t temp_pt_phys = pmm_alloc_frame();
    if (!temp_pt_phys) {
        return;
    }

    map_boot_page(temp_pt_phys);
    temp_page_table = (uint32_t*)(temp_pt_phys + KERNEL_BASE);
    memset(temp_page_table, 0, PAGE_SIZE);
    virt_page_directory[TEMP_PD_INDEX] = (temp_pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;

    invlpg((void*)TEMP_PT_VIRT);
}

void transition_page_directory() {
    // allocating new memory for the new directory
    uint32_t phys_page_directory = pmm_alloc_frame();
    map_boot_page(phys_page_directory);
    uint32_t* virt_page_directory = (uint32_t*)(phys_page_directory + KERNEL_BASE);
    memset(virt_page_directory, 0, PAGE_SIZE);
    
    // copy everything important over
    virt_page_directory[0] = kernel_page_directory[0];
    for (uint32_t entry = 768; entry < 1024; entry++) {
        virt_page_directory[entry] = kernel_page_directory[entry];
    }

    // must be before the switch because its allocated on the boot directory
    create_temp_page_table(virt_page_directory);
    
    // switch the directory
    load_cr3(phys_page_directory);

    kernel_page_directory = virt_page_directory;
    kernel_page_directory_phys = phys_page_directory;

}

uint32_t* temp_map_phys(uint32_t phys) {
    uint32_t virt = TEMP_PT_VIRT;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;
    
    temp_page_table[pt_idx] = (phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;
    invlpg((void*)virt);

    return (uint32_t*)virt;
}

uint32_t* create_page_table(uint32_t pd_idx) {
    uint32_t pt_phys = pmm_alloc_frame();
    if (!pt_phys) {
        return 0;
    }

    uint32_t* new_pt = temp_map_phys(pt_phys);
    memset(new_pt, 0, PAGE_SIZE);

    kernel_page_directory[pd_idx] = (pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;

    return new_pt;
}

void map_page(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = (uint32_t) virt >> 12 & 0x03FF;

    if (!(kernel_page_directory[pd_idx] & PAGE_PRESENT)) {
        if (!create_page_table(pd_idx)) {
            return;
        }
    }

    uint32_t pt_phys = kernel_page_directory[pd_idx] & 0xFFFFF000;
    uint32_t* pt = temp_map_phys(pt_phys);
    
    pt[pt_idx] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT;

    invlpg((void*)virt);
}
