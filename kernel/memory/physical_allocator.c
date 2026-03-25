#include "physical_allocator.h"
#include <stdio.h>

static pmm_t pmm;

static inline void bitmap_set(uint64_t frame) {
    pmm.bitmap[frame / 32] |= (1 << (frame % 32));
}

static inline void bitmap_clear(uint64_t frame) {
    pmm.bitmap[frame / 32] &= (1 << (frame % 32));
}

static inline bool bitmap_test(uint64_t frame) {
    return (pmm.bitmap[frame / 32] >> (frame % 32)) & 1;
}

void reserve_phys_range(uint32_t phys_start, uint32_t size) {
    uint32_t page_start = phys_start & ~(PAGE_SIZE - 1);
    uint32_t page_end = (phys_start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t page = page_start; page < page_end; page += PAGE_SIZE) {
        bitmap_set(page / PAGE_SIZE);
    }
}

void pmm_init(multiboot_info_t* mbi) {
    // 1. verify mbi has a memory map
    // 2. find top of physical memory
    // 3. choose where bitmap storage lives
    // 4. mark all frames used
    // 5. walk mmap and mark type==1 regions free
    // 6. reserve kernel image, boot page tables, bitmap, multiboot data, etc.    
    
    if (!(mbi->flags & (1 << 6))) {
        // no mmap present
        return; 
    }

    multiboot_mmap_entry_t* mmap = (multiboot_mmap_entry_t*)(mbi->mmap_addr + KERNEL_BASE);
    multiboot_mmap_entry_t* end = (multiboot_mmap_entry_t*)(mbi->mmap_addr + KERNEL_BASE + mbi->mmap_length);
    uint64_t max_phys = 0;
    
    while (mmap < end) {
        //mmap->addr  = physical base
        //mmap->len   = region length
        //mmap->type  = 1 means usable RAM

        uint64_t entry_end = mmap->addr + mmap->len;
        if (entry_end > max_phys) {
            max_phys = entry_end;
        }
        
        mmap = (multiboot_mmap_entry_t*) ((uint32_t)mmap + mmap->size + sizeof(uint32_t));
    }
    
    uint32_t total_frames = (max_phys + (PAGE_SIZE - 1)) / PAGE_SIZE;
    uint32_t bitmap_bytes = (total_frames + 7) / 8;
    uint32_t bitmap_pages = (bitmap_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    
    uintptr_t kernel_end_phys = (uintptr_t)&_kernel_end - KERNEL_BASE;
    uintptr_t bitmap_phys = ALIGN_UP(kernel_end_phys, PAGE_SIZE);
    
    // maps bitmap to virtual memory
    for (uint32_t page = bitmap_phys; page < bitmap_phys + bitmap_pages * PAGE_SIZE; page += PAGE_SIZE) {
        map_boot_page(page);
    } 

    uintptr_t bitmap_virt = bitmap_phys + KERNEL_BASE;


    pmm.total_frames = total_frames;
    pmm.bitmap = (uint32_t*)bitmap_virt;

    bitmap_phys = bitmap_phys & 0xFFFFF000;
    map_boot_page(bitmap_phys);

    for (uint32_t i = 0; i < (bitmap_bytes + 3) / 4; i++) {
        pmm.bitmap[i] = 0xFFFFFFFF;
    }

    mmap = (multiboot_mmap_entry_t*)(mbi->mmap_addr + KERNEL_BASE);
    end = (multiboot_mmap_entry_t*)(mbi->mmap_addr + KERNEL_BASE + mbi->mmap_length);
    
    // sets all regions in usuable RAM to be usuable
    while (mmap < end) {
        // usuable RAM memory 
        if (mmap->type == 1) {
            // causes first and last frames to not be included as they can be partially covered
            uint64_t region_start = ALIGN_UP(mmap->addr, PAGE_SIZE);
            uint64_t region_end = (mmap->addr + mmap->len) & ~(PAGE_SIZE - 1);

            uint64_t first_frame = region_start / PAGE_SIZE;
            uint64_t last_frame = region_end / PAGE_SIZE;

            // set all frames in the region to be unused
            for (uint64_t frame = first_frame; frame < last_frame; frame++) {
                bitmap_clear(frame); 
            }
        }        
        
        mmap = (multiboot_mmap_entry_t*) ((uint32_t)mmap + mmap->size + sizeof(uint32_t));
    }
/*    
    [0, 1 MiB)                         reserve for now
    [_kernel_start, _kernel_end)       reserve
    [boot_page_directory, +4096)       reserve
    [boot_page_table1, +4096)          reserve
    [bitmap_phys, bitmap_phys+size)    reserve
    [mbi_phys page(s)]                 reserve until copied
    [mmap_phys page(s)]                reserve until copied
*/

    // setting used memory as used
    bitmap_set(0);
    
    // set kernel to be used
    uint32_t kernel_start = (uint32_t)&_kernel_start - KERNEL_BASE;
    uint32_t kernel_end = (uint32_t)&_kernel_end - KERNEL_BASE;
    reserve_phys_range(kernel_start, kernel_end - kernel_start); 

    // set boot page directory to used
    uint32_t boot_page_directory_phys = (uint32_t)&boot_page_directory;
    reserve_phys_range(boot_page_directory_phys, PAGE_SIZE); 

    // set boot page table 1 to used
    uint32_t boot_page_table1_phys = (uint32_t)&boot_page_table1;
    reserve_phys_range(boot_page_table1_phys, PAGE_SIZE);

    // set the bitmap to used
    reserve_phys_range(bitmap_phys, bitmap_pages * PAGE_SIZE); 
    
    // set multiboot info to used
    reserve_phys_range((uint32_t)mbi, sizeof(multiboot_info_t));

    // set multiboot mmap entries to used
    reserve_phys_range(mbi->mmap_addr + KERNEL_BASE, mbi->mmap_length); 
}

uint32_t pmm_alloc_frame(void)  {          // returns physical address
    for (uint32_t frame = 0; frame < pmm.total_frames; frame++) {
        // frame is free
        if (!bitmap_test(frame)) {
            bitmap_set(frame);
            return frame * PAGE_SIZE;
        } 
    }

    return 0;
}

void pmm_free_frame(uint32_t phys_addr) {
    uint32_t frame = (phys_addr & ~(PAGE_SIZE - 1)) / PAGE_SIZE;
    bitmap_clear(frame);
}


