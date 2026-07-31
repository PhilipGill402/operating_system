#include <arch/memory/paging.h>

#include <arch/memory/physical_allocator.h>
#include <memory/address_space_allocator.h>
#include <memory/paging_defs.h>

#include <string.h>
#include <log.h>


static uint32_t* temp_page_table = NULL;
uint32_t next_free_virt = KERNEL_VIRT_START;

static struct arch_address_space kernel_space = {
    .directory_virt = (uint32_t*)&boot_page_directory,
    .directory_phys = (uint32_t)&boot_page_directory - KERNEL_BASE
};

static inline void i686_invlpg(void* addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(addr) : "memory");
}

static inline void i686_load_cr3(uint32_t phys) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(phys) : "memory");
}

static uint32_t i686_translate_page_flags(uint32_t flags) {
    uint32_t result = I686_PAGE_PRESENT;

    if ((flags & ARCH_PAGE_WRITE) != 0) {
        result |= I686_PAGE_WRITE;
    }

    if ((flags & ARCH_PAGE_USER) != 0) {
        result |= I686_PAGE_USER;
    }

    return result;
}

static uint32_t* create_page_table(arch_address_space_t* space, uint32_t pd_idx, uint32_t flags) {
    uint32_t pt_phys = pmm_alloc_frame();
    if (!pt_phys) {
        return 0;
    }

    uint32_t* new_pt = arch_phys_temp_map(5, pt_phys);
    if (!new_pt) {
        pmm_free_frame(pt_phys);
        return NULL;
    }

    memset(new_pt, 0, PAGE_SIZE);

    space->directory_virt[pd_idx] = (pt_phys & I686_ADDR_MASK) | I686_PAGE_PRESENT | I686_PAGE_WRITE | flags;

    arch_phys_temp_unmap(5);

    return new_pt;
}

static uint8_t create_shared_kernel_page_tables(uint32_t start, uint32_t end) {
    if (start >= end)
        return 0;

    uint32_t first_pde = start >> 22;
    uint32_t last_pde = (end - 1u) >> 22;

    for (uint32_t pde_idx = first_pde; pde_idx <= last_pde; pde_idx++) {
        if (kernel_space.directory_virt[pde_idx] & I686_PAGE_PRESENT)
            continue;

        if (!create_page_table(&kernel_space, pde_idx, I686_PAGE_PRESENT))
            return 0;
    }

    return 1;
}

/* *** I686 Specific Functions *** */
void i686_map_boot_page(uint32_t phys_addr) {
    if (phys_addr >= 0x400000) {
        log_error("map_boot_page phys too high: %x\n", phys_addr);
        for (;;);
    }

    uint32_t virt_addr = phys_addr + KERNEL_BASE;
    uint32_t pt_idx = (virt_addr >> 12) & 0x3FF;

    boot_page_table1[pt_idx] = (phys_addr & 0xFFFFF000) | I686_PAGE_PRESENT | I686_PAGE_WRITE;
    i686_invlpg((void*)virt_addr);
}

uint8_t i686_init_shared_region(void) {
    if (!create_shared_kernel_page_tables(PAGE_DIRECTORY_MAP_START, PAGE_DIRECTORY_MAP_END)) {
        log_error("failed to initialize shared kernel page tables");
        return 0;
    }

    if (!create_shared_kernel_page_tables(KERNEL_VIRT_START, KERNEL_VIRT_END)) {
        log_error("failed to initialize shared kernel page tables");
        return 0;
    }

    return 1;
}



/* *** TEMP PAGING *** */
void i686_create_temp_page_table() {
    uint32_t pt_phys = pmm_alloc_frame();

    if (!pt_phys) {
        log_error("failed to allocate temp map page table");
        return;
    }

    i686_map_boot_page(pt_phys);

    temp_page_table = (uint32_t*)(pt_phys + KERNEL_BASE);
    memset(temp_page_table, 0, PAGE_SIZE);

    kernel_space.directory_virt[TEMP_PD_INDEX] = pt_phys | I686_PAGE_PRESENT | I686_PAGE_WRITE;

    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 ::: "eax", "memory");
}

void* arch_phys_temp_map(uint32_t slot, uint32_t phys_addr) {
    if (!temp_page_table) {
        log_error("temp maps not initialized");
        return NULL;
    }

    if (slot >= TEMP_MAP_SLOTS) {
        log_error("temp map slot out of range");
        return NULL;
    }

    uint32_t virt = TEMP_MAP_BASE + slot * PAGE_SIZE;
    uint32_t pt_idx = (virt >> 12) & 0x03FF;

    uint32_t phys_page = phys_addr & 0xFFFFF000;
    uint32_t offset = phys_addr & 0x00000FFF;

    temp_page_table[pt_idx] = phys_page | I686_PAGE_PRESENT | I686_PAGE_WRITE;

    i686_invlpg((void*)virt);

    return (void*)(virt + offset);
}

void arch_phys_temp_unmap(uint32_t slot) {
    if (!temp_page_table || slot >= TEMP_MAP_SLOTS)
        return;

    uint32_t virt = TEMP_MAP_BASE + slot * PAGE_SIZE;
    uint32_t pt_idx = (virt >> 12) & 0x3FFu;

    temp_page_table[pt_idx] = 0;
    i686_invlpg((void *)(uintptr_t)virt);
}

/* *** INIT FUNCTIONS *** */
uint8_t arch_paging_transition() {
    // allocating new memory for the new directory
    uint32_t phys_page_directory = pmm_alloc_frame();
    if (!phys_page_directory) {
        return 0;
    }

    i686_map_boot_page(phys_page_directory);
    uint32_t* virt_page_directory = (uint32_t*)(phys_page_directory + KERNEL_BASE);
    memset(virt_page_directory, 0, PAGE_SIZE);
    
    // copy everything important over
    virt_page_directory[0] = kernel_space.directory_virt[0];
    for (uint32_t entry = 768; entry < 1024; entry++) {
        virt_page_directory[entry] = kernel_space.directory_virt[entry];
    }

    // switch the directory
    i686_load_cr3(phys_page_directory);
    
    kernel_space.directory_virt = virt_page_directory;
    kernel_space.directory_phys = phys_page_directory;

    return 1;
}

/* *** KERNEL ADDRESS SPACE *** */
arch_address_space_t* arch_kernel_address_space(void) {
    return &kernel_space;
}

/* *** ADDRESS SPACE MANAGEMENT *** */
uint8_t arch_address_space_create(arch_address_space_t* space) {
    uint32_t phys = pmm_alloc_frame();
    if (!phys) {
        return 0;
    }

    uint32_t* virt = (uint32_t*)get_address_space();
    if (!virt) {
        pmm_free_frame(phys);
        return 0;
    }

    if (!arch_page_map(&kernel_space, (uintptr_t)virt, phys, ARCH_PAGE_WRITE)) {
        release_address_space(virt); 
        pmm_free_frame(phys);
        return 0;
    }

    memset(virt, 0, PAGE_SIZE);

    // copy everything important over
    for (uint32_t entry = 768; entry < 1024; entry++) {
        virt[entry] = kernel_space.directory_virt[entry];
    }
    
    space->directory_virt = virt;
    space->directory_phys = phys;

    return 1;
}

void arch_address_space_destroy(arch_address_space_t* address_space) {
    if (!address_space || !address_space->directory_virt || !address_space->directory_phys)
        return;

    for (uint32_t pde_idx = 0; pde_idx < 768; pde_idx++) {
        uint32_t pde = address_space->directory_virt[pde_idx];
        if ((pde & I686_PAGE_PRESENT) == 0)
            continue;

        uint32_t pde_phys = pde & I686_ADDR_MASK;

        uint32_t* page_table = arch_phys_temp_map(5, pde_phys);
        
        // fix error handling (currently just leaking memory)
        if (!page_table)
            continue;

        for (uint32_t pte_idx = 0; pte_idx < 1024; pte_idx++) {
            uint32_t pte = page_table[pte_idx];


            if ((pte & I686_PAGE_PRESENT) == 0)
                continue;

            uint32_t pte_phys = pte & I686_ADDR_MASK;
            
            // TODO: Check if the address space owns this frame 
            pmm_free_frame(pte_phys);
            page_table[pte_idx] = 0;

        }

        address_space->directory_virt[pde_idx] = 0;
        
        arch_phys_temp_unmap(5);
        pmm_free_frame(pde_phys);
    } 

    arch_page_unmap(&kernel_space, (uint32_t)address_space->directory_virt);
    release_address_space(address_space->directory_virt);
    pmm_free_frame(address_space->directory_phys);

    address_space->directory_virt = NULL;
    address_space->directory_phys = 0;
}

uint8_t arch_address_space_activate(arch_address_space_t* address_space) {
    if (!address_space || !address_space->directory_phys)
        return 0;

    i686_load_cr3(address_space->directory_phys);
    
    return 1;
}

/* *** MEMORY PAGE OPERATIONS *** */
uint8_t arch_page_map(arch_address_space_t* space, uintptr_t virt, uint32_t phys, uint32_t flags) {
    if (!space || !space->directory_virt)
        return 0;

    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;

    if (!(space->directory_virt[pd_idx] & I686_PAGE_PRESENT)) {
        uint32_t table_flags = 0;

        if (flags & ARCH_PAGE_USER)
            table_flags |= I686_PAGE_USER;

        if (!create_page_table(space, pd_idx, table_flags))
            return 0;
    }
    
    uint32_t pt_phys = space->directory_virt[pd_idx] & I686_ADDR_MASK;
    uint32_t* pt = arch_phys_temp_map(0, pt_phys);
    if (!pt)
        return 0;
    
    // Page is already present
    if (pt[pt_idx] & I686_PAGE_PRESENT) {
        arch_phys_temp_unmap(0);
        return 0;
    }
    
    uint32_t i686_flags = i686_translate_page_flags(flags);
    pt[pt_idx] = (phys & I686_ADDR_MASK) | i686_flags;

    arch_phys_temp_unmap(0);

    i686_invlpg((void*)virt);

    return 1;
}

uint32_t arch_page_unmap(arch_address_space_t* space, uintptr_t virt) {
    if (!space || !space->directory_virt)
        return 0;

    uint32_t pd_idx = (uint32_t) virt >> 22;
    uint32_t pt_idx = ((uint32_t) virt >> 12) & 0x03FF;
    
    // page table doesnt exist
    if (!(space->directory_virt[pd_idx] & I686_PAGE_PRESENT)) {
        return 0;
    }

    uint32_t pt_phys = space->directory_virt[pd_idx] & I686_ADDR_MASK;
    uint32_t* pt = arch_phys_temp_map(0, pt_phys);
    if (!pt)
        return 0;
    
    // page table entry doesnt exist
    if (!(pt[pt_idx] & I686_PAGE_PRESENT)) {
        arch_phys_temp_unmap(0);
        return 0;
    }

    uint32_t phys = pt[pt_idx] & I686_ADDR_MASK;
    pt[pt_idx] = 0;
    
    arch_phys_temp_unmap(0);

    i686_invlpg((void*)virt);

    return phys;
}



