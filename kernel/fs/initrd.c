#include "fs/initrd.h"

initrd_superblock_t* superblock;
initrd_node_t* node_table;
uint32_t num_nodes;

//forward declaration
fs_node_t* initrd_parent(fs_node_t* node);

uint32_t initrd_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    initrd_node_t* file = &node_table[node->inode];
    
    uint32_t write_size = offset + size > node->size ? node->size - offset : size;
    if (offset + size > node->size) {
        return 0;
    }
    
    void* dst = (void*)((uint32_t)superblock + file->data_offset + offset);
    memcpy(dst, buffer, write_size);

    return write_size;
}

void initrd_createdir(fs_node_t* node, char* name) {
    // cant have duplicate files or directories of the same name 
    for (uint32_t i = 0; i < num_nodes; i++) {
        if (strcmp(name, node_table[i].name) == 0 && node->inode == node_table[i].parent_id) {
            return;
        }
    }

    initrd_node_t* new_dir = &node_table[num_nodes];
    new_dir->id = num_nodes;
    new_dir->type = INITRD_NODE_DIR;
    new_dir->parent_id = node->inode;
    strcpy(new_dir->name, name);
    new_dir->size = 0;
    new_dir->data_offset = 0;
    
    num_nodes++;
    superblock->node_count++;
}

void initrd_createfile(fs_node_t* node, char* name, uint32_t size) {
    // cant have duplicate files or directories of the same name 
    for (uint32_t i = 0; i < num_nodes; i++) {
        if (strcmp(name, node_table[i].name) == 0 && node->inode == node_table[i].parent_id) {
            return;
        }
    }
    
    initrd_node_t* new_file = &node_table[num_nodes];
    new_file->id = num_nodes;
    new_file->type = INITRD_NODE_FILE;
    new_file->parent_id = node->inode;
    strcpy(new_file->name, name);
    new_file->size = size;

    uint32_t max_offset = 0;
    for (uint32_t i = 0; i < num_nodes; i++) {
        initrd_node_t* node = &node_table[i];
        uint32_t offset = node->data_offset + node->size;
        if (offset > max_offset) {
            max_offset = offset;
        }
    }

    new_file->data_offset = max_offset;

    num_nodes++;
    superblock->node_count++;
}

uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (offset >= node->size) {
        return 0;
    }

    uint32_t bytes_to_read = size < (node->size - offset) ? size : (node->size - offset);
    
    if (node->inode > num_nodes) {
        log_error("invalide file id\n");
        return 0;
    }

    initrd_node_t* initrd_node = &node_table[node->inode];
    
    if (initrd_node->type != INITRD_NODE_FILE) {
        log_error("can't read a directory\n");
        return 0;
    }
    
    uint8_t* file = (uint8_t*)((uint32_t)superblock + initrd_node->data_offset + offset);
    memcpy(buffer, file, bytes_to_read);
    
    return bytes_to_read;
}

fs_dirent_t* initrd_readdir(fs_node_t* node, uint32_t index) {
    if (!node) return NULL;

    if (node->flags != FS_DIR) return NULL;

    fs_dirent_t* entry = kmalloc(sizeof(fs_dirent_t));
    if (!entry) return NULL;
    
    for (uint32_t idx = 0, count = 0; idx < num_nodes; idx++) {
        initrd_node_t* file = &node_table[idx];

        if (file->parent_id == node->inode) {
            if (count == index) {
                strcpy(entry->name, file->name);
                entry->inode = file->id;
                
                return entry;
            }

            count++;
        }
    }
    
    return NULL; 
}

fs_node_t* initrd_finddir(fs_node_t* node, char* name) {
    for (uint32_t i = 0; i < superblock->node_count; i++) {
        initrd_node_t* curr_node = &node_table[i];
        if (strcmp(curr_node->name, name) == 0 && curr_node->parent_id == node->inode) {
            fs_node_t* file = kmalloc(sizeof(fs_node_t));
            strcpy(file->name, curr_node->name);
            file->flags = curr_node->type == INITRD_NODE_DIR ? FS_DIR : FS_FILE;
            file->inode = curr_node->id;
            file->size = curr_node->size;
            file->read = initrd_read;
            file->finddir = initrd_finddir;
            file->readdir = initrd_readdir;
            file->parent = initrd_parent;
            file->createdir = initrd_createdir;
            file->createfile = initrd_createfile;
            file->writefile = initrd_writefile;
            
            return file;
        }
    }
    
    return NULL;
}

fs_node_t* initrd_parent(fs_node_t* node) {
    initrd_node_t* parent_initrd_node;
    if (node->inode == superblock->root_node) {
        parent_initrd_node = &node_table[0];
    } else {
        initrd_node_t* initrd_node = &node_table[node->inode];
        parent_initrd_node = &node_table[initrd_node->parent_id];
    }


    fs_node_t* fs_node = kmalloc(sizeof(fs_node_t));
    strcpy(fs_node->name, parent_initrd_node->name);
    fs_node->flags = parent_initrd_node->type == INITRD_NODE_DIR ? FS_DIR : FS_FILE;
    fs_node->inode = parent_initrd_node->id;
    fs_node->size = parent_initrd_node->size;
    fs_node->read = initrd_read;
    fs_node->finddir = initrd_finddir;
    fs_node->readdir = initrd_readdir;
    fs_node->parent = initrd_parent;
    fs_node->createdir = initrd_createdir;
    fs_node->createfile = initrd_createfile;

    return fs_node;
}

fs_node_t* initrd_init(uint32_t addr) {
    // set and validate superblock 
    superblock = (initrd_superblock_t*)addr;
    
    if (superblock->magic != INITRD_MAGIC) {
        log_error("image file not in specified format\n"); 
        return NULL;
    }

    if (superblock->node_count == 0) {
        log_error("file system must have at least one file\n");
        return NULL;
    }
    
    // set node table and transfer initial node table to a heap based one
    initrd_node_t* init_node_table = (initrd_node_t*)(addr + superblock->nodes_offset);
    node_table = kmalloc(100*sizeof(initrd_node_t));
    
    for (uint32_t i = 0; i < superblock->node_count; i++) {
        node_table[i] = init_node_table[i];
    }
    
    // set and validate root node
    initrd_node_t root = node_table[superblock->root_node];

    if (root.id != 0) {
        log_error("root node not 0\n");
        return NULL;
    }

    if (root.type != INITRD_NODE_DIR) {
        log_error("root node is not a directory\n");
        return NULL;
    }

    if (root.parent_id != INITRD_INVALID_NODE) {
        log_error("root node parent is not invalid\n");
        return NULL;
    }

    if (strcmp(root.name, "/")) {
        log_error("root name is not correct\n");
        return NULL;
    }
    
    // set number of nodes
    num_nodes = superblock->node_count;
     
    // set root node
    fs_node_t* fs_root = kmalloc(sizeof(fs_node_t));
    strcpy(fs_root->name, root.name);
    fs_root->flags = FS_DIR;
    fs_root->inode = root.id;
    fs_root->size = root.size;
    fs_root->read = initrd_read;
    fs_root->readdir = initrd_readdir;
    fs_root->finddir = initrd_finddir;
    fs_root->parent = initrd_parent;
    fs_root->createdir = initrd_createdir;
    fs_root->createfile = initrd_createfile;
    
    return fs_root;
}


