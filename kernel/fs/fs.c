#include "fs/fs.h"

uint8_t fs_poll(fs_node_t* node, uint32_t offset) {
    if (!node || !node->poll)
        return POLLERR;

    if (node->flags & FS_DIR)
        return 0;

    return node->poll(node, offset);
}

fs_node_t* fs_parent(fs_node_t* node) {
    if (!node || !node->parent) {
        log_error("node is at %x or node->parent is null\n", node); 
        return NULL;
    } 
    
    if (node->mount_parent) {
        return node->mount_parent;
    }
        
    return node->parent(node);
}

void fs_createdir(fs_node_t* node, char* name) {
    if (!node || !node->createdir) {
        log_error("node is at %x or node->createdir is null\n", node); 
        return;
    }

    if (!name) {
        log_error("name is at %x\n", name);
        return;
    }
        
    node->createdir(node, name);
}

void fs_createfile(fs_node_t* node, char* name, uint32_t size) {
    if (!node || !node->createfile) {
        log_error("node is at %x or node->createfile is null\n", node); 
        return;
    }
        

    return node->createfile(node, name, size);
}

int32_t fs_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    if (!node) {
        log_error("node is at %x\n", node);
        return 0;
    }
        

    if (!node->writefile || node->flags == FS_DIR) {
        log_error("node->writefile is at %x and node->flags == FS_DIR = %d\n", node->writefile, node->flags == FS_DIR);        
        return 0;
    }
        

    return node->writefile(node, buffer, offset, size);
}

int32_t fs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !node->read) {
        log_error("node is at %x or node->read is null\n", node);
        return 0;
    }

    return node->read(node, offset, size, buffer);
}

fs_dirent_t* fs_readdir(fs_node_t* node, uint32_t index) {
    if (!node || !node->readdir) {
        return NULL;
    }
    
    return node->readdir(node, index);
}

fs_node_t* fs_finddir(fs_node_t* node, char* name) {
    if (!node || !node->finddir) {
        log_error("node is at %x or node->finddir is null\n", node);
        return NULL;
    }
    
    return node->finddir(node, name);
}

uint8_t fs_init(multiboot_info_t* mbi) {
    if (!(mbi->flags & (1 << 3))) {
        log_error("multiboot info not found\n");
        return 0;
    }

    if (mbi->mods_count == 0) {
        log_error("mods_count is 0\n");
        return 0;
    }
    
    uint32_t mods_start = mbi->mods_addr;
    uint32_t mods_end = mbi->mods_addr + (mbi->mods_count * sizeof(multiboot_module_t));

    uint32_t start = mods_start & ~(PAGE_SIZE - 1);
    uint32_t end = (mods_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    if (end > MULTIBOOT_VIRT_END) {
        log_error("multiboot module table mapping too large\n");
        return 0;
    }

    for (uint32_t page = start, offset = 0; page < end; page += PAGE_SIZE, offset += PAGE_SIZE) {
        if (!arch_page_map(arch_kernel_address_space(), MULTIBOOT_VIRT_START + offset, page, ARCH_PAGE_WRITE))
            return 0;
    }
    
    uint32_t mods_offset = mods_start & (PAGE_SIZE - 1);
    multiboot_module_t* modules = (multiboot_module_t*)(MULTIBOOT_VIRT_START + mods_offset);
    multiboot_module_t* initrd_mod = &modules[0];
    
    uint32_t mod_start = initrd_mod->mod_start;
    uint32_t mod_end = initrd_mod->mod_end;

    start = mod_start & ~(PAGE_SIZE - 1);
    end = (mod_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    if (end > INITRD_VIRT_END) {
        log_error("initrd module mapping too large\n");
        return 0;
    }

    for (uint32_t page = start, offset = 0; page < end; page += PAGE_SIZE, offset += PAGE_SIZE) {
        if (!arch_page_map(arch_kernel_address_space(), INITRD_VIRT_ADDRESS + offset, page, ARCH_PAGE_WRITE))
            return 0;
    }

    uint32_t initrd_location = INITRD_VIRT_ADDRESS + (mod_start & 0xFFF);
    
    fs_root = vfs_init();
    if (!fs_root) {
        log_error("vfs_init failed\n");
        return 0;
    }

    fs_cwd = fs_root;
    
    fs_node_t* initrd_root = initrd_init(initrd_location);
    if (!initrd_root) {
        log_error("initrd_init failed\n");
        return 0;
    }
    fs_mount(initrd_root, "bin");

    fs_node_t* ramfs = ramfs_init();
    if (!ramfs) {
        log_error("ramfs_init failed\n");
        return 0;
    }

    fs_mount(ramfs, "tmp");

    fs_node_t* dev_dir = init_dev();
    if (!dev_dir) {
        log_error("dev_init failed\n");
        return 0;
    } 

    fs_mount(dev_dir, "dev");

    fs_node_t* proc_dir = init_proc();
    if (!proc_dir) {
        log_error("proc_init failed\n");
        return 0;
    }

    fs_mount(proc_dir, "proc");

    return 1;
}

fs_node_t* resolve_path_from(fs_node_t* start, const char* path) {
    if (!start || !path) 
        return NULL;
        
    fs_node_t* current = NULL;
    
    if (path[0] == '/') {
        current = fs_node_clone(fs_root);
        while (*path == '/')
            path++;
    } else {
         current = fs_node_clone(start);
    }

    char component[128];
    
    while (*path) {
        while (*path == '/') {
            path++;
        }

        if (*path == '\0') {
            break;
        }

        uint32_t i = 0;

        while (*path && *path != '/' && i < sizeof(component) - 1) {
            component[i++] = *path++;
        }

        component[i] = '\0';

        if (strcmp(component, ".") == 0) {
            continue;
        }

        if (strcmp(component, "..") == 0) {
            fs_node_t* parent = fs_parent(current);

            if (parent) {
                kfree(current); 
                current = parent;
            }
            
            continue;
        }

        fs_node_t* old_current = current;
        current = fs_finddir(current, component);
        kfree(old_current);

        if (!current) {
            log_error("couldn't resolve path: %s\n", path);
            return NULL;
        }
    }
    
    return current;
}

fs_node_t* resolve_path(const char* path, fs_node_t* cwd) {
    if (!path) {
        log_error("path is null\n"); 
        return NULL;
    } 
        
    if (path[0] == '/') {
        return resolve_path_from(fs_root, path);
    }
    
    return resolve_path_from(cwd, path);
}



