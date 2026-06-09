#include "memory/paging.h"

static uint32_t* temp_page_table = NULL;
uint32_t* kernel_page_directory = (uint32_t*)&boot_page_directory;
uint32_t kernel_page_directory_phys = (uint32_t)&boot_page_directory - KERNEL_BASE;
uint32_t next_free_virt = KERNEL_VIRT_START;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
}

void* map_phys_range_to_virt(uint32_t virt_base, uint32_t virt_limit, uint32_t phys_start, uint32_t length, uint32_t flags) {
    uint32_t phys_offset = phys_start & (PAGE_SIZE - 1);
    uint32_t phys_page = phys_start & ~(PAGE_SIZE - 1);
    uint32_t map_size = align_up(length + phys_offset, PAGE_SIZE);

    if (virt_base + map_size > virt_limit) {
        log_error("map_phys_range_to_virt: range too large virt=%x size=%x limit=%x\n",
                  virt_base, map_size, virt_limit);
        return NULL;
    }

    for (uint32_t off = 0; off < map_size; off += PAGE_SIZE) {
        map_page(virt_base + off, phys_page + off, PAGE_PRESENT | flags);
    }

    return (void*)(virt_base + phys_offset);
}

void map_boot_page(uint32_t phys_addr) {
    if (phys_addr >= 0x400000) {
        log_error("map_boot_page phys too high: %x\n", phys_addr);
        for (;;);
    }

    uint32_t virt_addr = phys_addr + KERNEL_BASE;
    uint32_t pt_idx = (virt_addr >> 12) & 0x3FF;

    boot_page_table1[pt_idx] = (phys_addr & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;
    invlpg((void*)virt_addr);
}

void create_temp_page_table(uint32_t* virt_page_directory, uint32_t pd_idx) {
    uint32_t pt_phys = pmm_alloc_frame();
    if (!pt_phys) {
        log_error("failed to allocate frame\n"); 
        return;
    }

    map_boot_page(pt_phys);

    temp_page_table = (uint32_t*)(pt_phys + KERNEL_BASE);
    memset(temp_page_table, 0, PAGE_SIZE);

    kernel_page_directory[TEMP_PD_INDEX] = (pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;
}

static void* temp_map_phys_slot(uint32_t slot, uint32_t phys) {
    uint32_t virt = TEMP_MAP_BASE + slot * PAGE_SIZE;
    uint32_t pt_idx = (virt >> 12) & 0x3FF;

    temp_page_table[pt_idx] = (phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE;

    invlpg((void*)virt);

    return (void*)(virt + (phys & 0xFFF));
}

void paging_init_temp_regions() {
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_0);
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_1);
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_2);
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_3);
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_4);
    create_temp_page_table(kernel_page_directory, TEMP_PD_INDEX_5);
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

    // switch the directory
    load_cr3(phys_page_directory);

    kernel_page_directory = virt_page_directory;
    kernel_page_directory_phys = phys_page_directory;

}

uint32_t* temp_map_phys0(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(0, phys);
}

uint32_t* temp_map_phys1(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(1, phys);
}

uint32_t* temp_map_phys2(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(2, phys);
}

uint32_t* temp_map_phys3(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(3, phys);
}

uint32_t* temp_map_phys4(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(4, phys);
}

uint32_t* temp_map_phys5(uint32_t phys) {
    return (uint32_t*)temp_map_phys_slot(5, phys);
}

uint32_t* create_page_table(uint32_t* page_directory, uint32_t pd_idx, uint32_t flags) {
    uint32_t pt_phys = pmm_alloc_frame();
    if (!pt_phys) {
        return 0;
    }

    uint32_t* new_pt = temp_map_phys5(pt_phys);
    memset(new_pt, 0, PAGE_SIZE);

    page_directory[pd_idx] = (pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_WRITE | flags;

    return new_pt;
}

void map_page(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;

    if (!(kernel_page_directory[pd_idx] & PAGE_PRESENT)) {
        if (!create_page_table(kernel_page_directory, pd_idx, 0)) {
            return;
        }
    }
    
    uint32_t pt_phys = kernel_page_directory[pd_idx] & 0xFFFFF000;
    uint32_t* pt = temp_map_phys0(pt_phys);

    pt[pt_idx] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT;

    invlpg((void*)virt);
}

void map_user_page(uint32_t* page_directory, uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;

    if (!(page_directory[pd_idx] & PAGE_PRESENT)) {
        if (!create_page_table(page_directory, pd_idx, PAGE_USER)) {
            return;
        }
    } else if (!(page_directory[pd_idx] & PAGE_USER)) {
        return;
    }

    uint32_t pt_phys = page_directory[pd_idx] & 0xFFFFF000;
    uint32_t* pt = temp_map_phys5(pt_phys);

    pt[pt_idx] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT | PAGE_USER;
        
    invlpg((void*)virt);
}

uint32_t unmap_page(uint32_t virt) {
    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;
    
    // page table doesnt exist
    if (!(kernel_page_directory[pd_idx] & PAGE_PRESENT)) {
        return 0;
    }

    uint32_t pt_phys = kernel_page_directory[pd_idx] & 0xFFFFF000;
    uint32_t* pt = temp_map_phys0(pt_phys);
    
    // page table entry doesnt exist
    if (!(pt[pt_idx] & PAGE_PRESENT)) {
        return 0;
    }

    uint32_t phys = pt[pt_idx] & 0xFFFFF000;
    pt[pt_idx] = 0;

    invlpg((void*)virt);

    return phys;
}

void* alloc_kernel_page(uint32_t flags) {
    if (next_free_virt + PAGE_SIZE > KERNEL_VIRT_END) {
        return NULL;
    }

    uint32_t frame = pmm_alloc_frame();
    if (!frame) {
        return 0;
    }

    uint32_t virt = next_free_virt;
    next_free_virt += PAGE_SIZE;

    map_page(virt, frame, flags);

    return (void*)virt;
}

void free_kernel_page(uint32_t virt) {
    uint32_t phys = unmap_page(virt);

    if (phys) {
        pmm_free_frame(phys);
    }
}
