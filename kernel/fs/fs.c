#include "fs/fs.h"

fs_node_t* fs_root;
fs_node_t* fs_cwd;
fs_node_t* dev_dir;
fs_node_t* console_node;

fs_node_t* resolve_path_from(fs_node_t* start, const char* path) {
    fs_node_t* start_copy = kmalloc(sizeof(fs_node_t));
    memcpy(start_copy, start, sizeof(fs_node_t));
    strcpy(start_copy->name, start->name); 

    if (path[0] == '/') {
        start_copy = fs_root; 
    }
    
    char path_copy[MAX_PATH_LENGTH];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* curr_dir = strtok(path_copy, '/');

    if (!curr_dir && strcmp(path, "/") == 0) {
        return start_copy;
    }

    while (curr_dir != NULL) {
        if (strcmp(curr_dir, ".") == 0) {
            curr_dir = strtok(NULL, '/');
            continue;
        } else if (strcmp(curr_dir, "..") == 0) {
            start_copy = fs_parent(start_copy);
            curr_dir = strtok(NULL, '/');
            continue;
        }
        
        fs_node_t* dir = fs_finddir(start_copy, curr_dir);

        if (!dir) {
            log_error("'dir' returned null\n");
            return NULL;
        }
            

        start_copy = dir;
        curr_dir = strtok(NULL, '/');
    }

    return start_copy;
}

fs_node_t* resolve_path(const char* path) {
    if (!path) {
        log_error("path is null\n"); 
        return NULL;
    } 
        
    

    if (path[0] == '/') {
        return resolve_path_from(fs_root, path);
    }
    
    return resolve_path_from(fs_cwd, path);
}

fs_node_t* fs_parent(fs_node_t* node) {
    if (!node || !node->parent) {
        log_error("node is at %p or node->parent is null\n", node); 
        return NULL;
    } 
        
    

    return node->parent(node);
}

void fs_createdir(fs_node_t* node, char* name) {
    if (!node || !node->createdir) {
        log_error("node is at %p or node->createdir is null\n", node); 
        return;
    }
        

    return node->createdir(node, name);
}

void fs_createfile(fs_node_t* node, char* name, uint32_t size) {
    if (!node || !node->createfile) {
        log_error("node is at %p or node->createfile is null\n", node); 
        return;
    }
        

    return node->createfile(node, name, size);
}

uint32_t fs_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    if (!node) {
        log_error("node is at %p\n");
        return 0;
    }
        

    if (!node->writefile || node->flags == FS_DIR) {
        log_error("node->writefile is at %p and node->flags == FS_DIR = %d\n", node->writefile, node->flags == FS_DIR);        
        return 0;
    }
        

    return node->writefile(node, buffer, offset, size);
}

uint32_t fs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !node->read) {
        log_error("node is at %p or node->read is null\n", node); 
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
        log_error("node is at %p or node->finddir is null\n", node);
        return NULL;
    }
        

    if (node == fs_root && strcmp(name, "dev") == 0) {
        return dev_dir; 
    }

    return node->finddir(node, name);
}

fs_dirent_t* dev_readdir(fs_node_t* node, uint32_t index) {
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t)); 
    dev_dir_t* dir = (dev_dir_t*)node->device;
    
    if (!dir || index >= dir->child_count) {
        log_error("dir is at %p or index >= dir->child_count\n", node);
        return NULL;
    }
        

    fs_node_t* child = dir->children[index];
    strcpy(dirent->name, child->name);
    dirent->inode = child->inode;

    return dirent;
}

fs_node_t* dev_finddir(fs_node_t* node, char* name) {
    dev_dir_t* dir = (dev_dir_t*)node->device;

    if (!dir) {
        log_error("dir is at null\n");
        return NULL;
    } 
        

    for (uint32_t i = 0; i < dir->child_count; i++) {
        if (strcmp(dir->children[i]->name, name) == 0) {
            return dir->children[i];
        }
    }

    return NULL;
}

fs_node_t* dev_parent(fs_node_t* node) {
    if (!node || !node->device) {
        log_error("node is at %p or node->device is\n", node);
        return NULL;
    }
        
    
    dev_dir_t* dir = node->device;
    
    return dir->parent;
}

fs_node_t* create_dev_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    dev_dir_t* data = kzmalloc(sizeof(dev_dir_t));

    if (!node || !data) {
        log_error("node is at %p and data is at %p\n", node, data);
        return NULL;
    }
        

    strcpy(node->name, name);
    node->flags = FS_DIR;
    node->inode = inode;
    node->size = 0;
    node->device = data;

    data->child_count = 0;
    data->parent = parent;

    node->read = NULL;
    node->readdir = dev_readdir;
    node->finddir = dev_finddir;
    node->parent = dev_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = NULL;

    return node;
}

int dev_add_child(fs_node_t* dir, fs_node_t* child) {
    if (!dir || !child) {
        log_error("dir is at %x and child is at %x\n", dir, child);
        return 0;
    }
        

    dev_dir_t* device = dir->device;
    
    if (!device || device->child_count >= 16) {
        log_error("device is at %x or device->child_count >= 16\n", device);
        return 0;
    }
        

    device->children[device->child_count++] = child;

    return 1;
}

void init_dev() {
    dev_dir = create_dev_dir("dev", fs_root, num_nodes);
    console_node = create_console_node(dev_dir);

    if (!dev_add_child(dev_dir, console_node))
        log_error("failed to init devices\n");
}

uint8_t fs_init(multiboot_info_t* mbi, fs_node_t* (*init)(uint32_t addr)) {
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
    fs_cwd = fs_root;

    if (!fs_root) {
        log_error("initrd_init failed\n");
        return 0;
    }

    init_dev();

    return 1;
}


