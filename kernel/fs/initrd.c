#include "fs/initrd.h"

initrd_superblock_t* superblock;
initrd_node_t* node_table;
uint32_t num_nodes;

//forward declaration
fs_node_t* initrd_parent(fs_node_t* node);

uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (offset >= node->size) {
        return 0;
    }

    uint32_t bytes_to_read = size < (node->size - offset) ? size : (node->size - offset);
    
    if (node->inode > num_nodes) {
        printf("invalide file id\n");
        return 0;
    }

    initrd_node_t* initrd_node = &node_table[node->inode];
    
    if (initrd_node->type != INITRD_NODE_FILE) {
        printf("can't read a directory\n");
        return 0;
    }

    uint8_t* file = (uint8_t*)((uint32_t)superblock + initrd_node->data_offset + offset);
    memcpy(buffer, file, bytes_to_read);
    buffer[bytes_to_read] = '\0';
    
    return bytes_to_read;
}

dirent_t* initrd_readdir(fs_node_t* node, uint32_t index) {
    if (node->flags != FS_DIR) {
        return NULL;
    } 
    
    dirent_t* entry;
    for (uint32_t idx = 0, count = 0; idx < num_nodes; idx++) {
        initrd_node_t* file = &node_table[idx];

        if (file->parent_id == node->inode) {
            if (count == index) {
                entry = kmalloc(sizeof(dirent_t)); 
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

    return fs_node;
}

fs_node_t* initrd_init(uint32_t addr) {
    // set and validate superblock 
    superblock = (initrd_superblock_t*)addr;
    
    if (superblock->magic != INITRD_MAGIC) {
        printf("image file not in specified format\n"); 
        return NULL;
    }

    if (superblock->node_count == 0) {
        printf("file system must have at least one file\n");
        return NULL;
    }
    
    // set node table
    node_table = (initrd_node_t*)(addr + superblock->nodes_offset);
    
    // set and validate root node
    initrd_node_t root = node_table[superblock->root_node];

    if (root.id != 0) {
        printf("root node not 0\n");
        return NULL;
    }

    if (root.type != INITRD_NODE_DIR) {
        printf("root node is not a directory\n");
        return NULL;
    }

    if (root.parent_id != INITRD_INVALID_NODE) {
        printf("root node parent is not invalid\n");
        return NULL;
    }

    if (strcmp(root.name, "/")) {
        printf("root name is not correct\n");
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
    return fs_root;
}


