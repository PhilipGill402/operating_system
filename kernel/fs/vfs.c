#include "fs/vfs.h"

mount_t mount_table[MAX_MOUNTS];
uint8_t mount_count = 0;

int fs_mount(fs_node_t* root, char* name) {
    if (mount_count >= MAX_MOUNTS)
        return -1;
    
    mount_t mount;
    strncpy(mount.name, name, 128);
    mount.root = root;
    root->mount_parent = fs_node_clone(fs_root);
    
    mount_table[mount_count++] = mount;

    return 0;
}

fs_dirent_t* vfs_readdir(fs_node_t* node, uint32_t index) {
    if (index >= mount_count)
        return NULL;

    mount_t mount = mount_table[index];
    
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t));
    strcpy(dirent->name, mount.name);
    dirent->inode = index;

    return dirent;
}

fs_node_t* vfs_finddir(fs_node_t* node, char* name) {
    if (!name)
        return NULL;

    for (uint8_t i = 0; i < mount_count; i++) {
        if (strcmp(mount_table[i].name, name) == 0) {
            return fs_node_clone(mount_table[i].root);
        }
    }

    return NULL;
}

fs_node_t* vfs_parent(fs_node_t* node) {
    return fs_node_clone(node);
}

fs_node_t* vfs_init() {
    fs_node_t* vfs_root = kmalloc(sizeof(fs_node_t));
    strcpy(vfs_root->name, "/");
    vfs_root->flags = FS_DIR;
    vfs_root->inode = inode_count++;
    vfs_root->size = 0;
    vfs_root->device = NULL;
    vfs_root->proc = NULL;
    vfs_root->mount_parent = NULL;

    vfs_root->read = NULL;
    vfs_root->readdir = vfs_readdir;
    vfs_root->finddir = vfs_finddir;
    vfs_root->parent = vfs_parent;
    vfs_root->createdir = NULL;
    vfs_root->createfile = NULL;
    vfs_root->writefile = NULL;

    return vfs_root;
}
