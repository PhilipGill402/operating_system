#include "fs/fs.h"

fs_node_t* fs_root;

uint32_t fs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !node->read) {
        return 0;
    } 

    return node->read(node, offset, size, buffer);
}

dirent_t fs_readdir(fs_node_t* node, uint32_t index) {
    if (!node || !node->readdir) {
        dirent_t entry;
        entry.inode = (uint32_t)-1;
        return entry;
    }

    return node->readdir(node, index);
}

fs_node_t* fs_finddir(fs_node_t* node, char* name) {
    if (!node || !node->finddir) {
        return NULL;
    }

    return node->finddir(node, name);
}

uint8_t fs_init(multiboot_info_t* mbi, fs_node_t* (*init)(uint32_t addr)) {
    if (!(mbi->flags & (1 << 3))) {
        printf("No multiboot modules found\n");
        return 0;
    }

    if (mbi->mods_count == 0) {
        printf("mods_count is 0\n");
        return 0;
    }
    
    uint32_t mods_start = mbi->mods_addr;
    uint32_t mods_end = mbi->mods_addr + (mbi->mods_count * sizeof(multiboot_module_t));

    uint32_t start = mods_start & ~(PAGE_SIZE - 1);
    uint32_t end = (mods_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t page = start; page < end; page += PAGE_SIZE) {
        map_page(page + KERNEL_BASE, page, PAGE_WRITE);
    }

    multiboot_module_t* modules = (multiboot_module_t*)(mbi->mods_addr + KERNEL_BASE);
    multiboot_module_t* initrd_mod = &modules[0];
    
    uint32_t mod_start = initrd_mod->mod_start;
    uint32_t mod_end = initrd_mod->mod_end;

    start = mod_start & ~(PAGE_SIZE - 1);
    end = (mod_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t page = start; page < end; page += PAGE_SIZE) {
        map_page(page + KERNEL_BASE, page, PAGE_WRITE);
    }

    uint32_t initrd_location = mod_start + KERNEL_BASE;
    
    fs_root = initrd_init(initrd_location);

    if (!fs_root) {
        printf("initrd_init failed\n");
        return 0;
    }

    printf("Filesystem initialized\n");
    return 1;
}
