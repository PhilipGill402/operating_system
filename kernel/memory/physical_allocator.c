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
    
    // setting used memory as used
    bitmap_set(0);
    
    // set kernel to be used
    uint32_t kernel_start = (uint32_t)&_kernel_start - KERNEL_BASE;
    uint32_t kernel_end = (uint32_t)&_kernel_end - KERNEL_BASE;
    
    uint32_t kernel_start_frame = kernel_start / PAGE_SIZE;
    uint32_t kernel_end_frame = (kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t frame = kernel_start_frame; frame < kernel_end_frame; frame++) {
        bitmap_set(frame);
    } 

    // set boot page directory to used
    uint32_t boot_page_directory_phys = (uint32_t)&boot_page_directory;
    bitmap_set(boot_page_directory_phys / PAGE_SIZE); 

}

uint32_t pmm_alloc_frame(void);          // returns physical address
void pmm_free_frame(uint32_t phys_addr);
bool pmm_is_used(uint32_t frame_index);
void pmm_mark_used(uint32_t phys_addr);
void pmm_mark_free(uint32_t phys_addr);
