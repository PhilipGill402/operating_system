#include "fs/devfs/devfs.h"

int32_t dev_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
int32_t dev_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size);
fs_dirent_t* dev_readdir(fs_node_t* node, uint32_t index);
fs_node_t* dev_finddir(fs_node_t* node, char* name);
fs_node_t* dev_parent(fs_node_t* node);

pty_t pty;

void pty_init(void) {
    pty.input = queue_create(sizeof(char));
    pty.output = queue_create(sizeof(char));
}

fs_node_t* dev_file_to_node(dev_file_t* file) {
    if (!file)
        return NULL;

    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node) {
        kfree(node);
        return NULL;
    }

    strcpy(node->name, file->name);
    node->flags = FS_FILE;
    node->inode = file->inode;
    node->size = 0;
    node->device = file;
    
    node->read = dev_read;
    node->readdir = NULL;
    node->finddir = NULL;
    node->parent = dev_parent;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = dev_writefile;
    
    return node;
}

int32_t dev_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !buffer)
        return 0;
    
    if (node->flags != FS_FILE)
        return 0;
    
    dev_file_t* file = (dev_file_t*)node->device;
    if (!file)
        return 0;
    
    int32_t len = file->get_data(file, buffer, offset, size);    
    
    return len;
}

int32_t dev_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    if (!node || !buffer)
        return 0;
    
    if (node->flags != FS_FILE)
        return 0;
    
    dev_file_t* file = (dev_file_t*)node->device;
    if (!file)
        return 0;
    
    int32_t len = file->write_data(file, buffer, offset, size);    
    
    return len;
}

fs_dirent_t* dev_readdir(fs_node_t* node, uint32_t index) {
    fs_dirent_t* dirent = kmalloc(sizeof(fs_dirent_t)); 
    dev_dir_t* dir = (dev_dir_t*)node->device;
    
    if (!dir || index >= dir->child_count) {
        log_error("dir is at %x or index >= dir->child_count\n", node);
        kfree(dirent);
        return NULL;
    }
        

    dev_file_t* child = dir->children[index];
    strcpy(dirent->name, child->name);
    dirent->inode = child->inode;

    return dirent;
}

fs_node_t* dev_finddir(fs_node_t* node, char* name) {
    if (!node || !name) return NULL; 

    dev_dir_t* dir = (dev_dir_t*)node->device;
    fs_node_t* ret = kmalloc(sizeof(fs_node_t));

    if (!dir) {
        log_error("dir is at null\n");
        kfree(ret);
        return NULL;
    } 
        
    for (uint32_t i = 0; i < dir->child_count; i++) {
        dev_file_t* child = dir->children[i];
        if (strcmp(child->name, name) == 0) {
            return dev_file_to_node(child);
        }
    }
    
    kfree(ret);
    return NULL;
}

fs_node_t* dev_parent(fs_node_t* node) {
    if (!node || !node->device) {
        log_error("node is at %x or node->device is\n", node);
        return NULL;
    }
    
    dev_dir_t* dir = node->device;

    if (!dir) return NULL;
    
    return fs_node_clone(dir->parent);
}

fs_node_t* create_dev_dir(const char* name, fs_node_t* parent, uint32_t inode) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));
    dev_dir_t* data = kzmalloc(sizeof(dev_dir_t));

    if (!node || !data) {
        log_error("node is at %x and data is at %x\n", node, data);
        kfree(node); 
        kfree(data); 
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

int dev_add_child(fs_node_t* dir, dev_file_t* child) {
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

fs_node_t* init_dev() {
    fs_node_t* dev_dir = create_dev_dir("dev", fs_root, inode_count++);
    
    pty_init();

    dev_file_t* console_file = create_console_file(dev_dir, inode_count++);
    if (!console_file)
        log_error("failed to init console\n");

    if (!dev_add_child(dev_dir, console_file))
        log_error("failed to init devices\n");

    dev_file_t* fb_file = create_fb_file(dev_dir, inode_count++);
    if (!fb_file)
        log_error("failed to init framebuffer file\n");

    if (!dev_add_child(dev_dir, fb_file))
        log_error("failed to init devices\n");
    
    dev_file_t* serial_file = create_serial_file(dev_dir, inode_count++);
    if (!serial_file)
        log_error("failed to init serial file\n");

    if (!dev_add_child(dev_dir, serial_file))
        log_error("failed to init devices\n");
    
    dev_file_t* ptm_file = create_ptm_file(dev_dir, inode_count++);
    if (!ptm_file)
        log_error("failed to init ptm file\n");

    if (!dev_add_child(dev_dir, ptm_file))
        log_error("failed to init devices\n");

    dev_file_t* pts_file = create_pts_file(dev_dir, inode_count++);
    if (!pts_file)
        log_error("failed to init pts file\n");

    if (!dev_add_child(dev_dir, pts_file))
        log_error("failed to init devices\n");

    dev_file_t* input_file = create_input_file(dev_dir, inode_count++);
    if (!input_file)
        log_error("failed to init input file\n");

    if (!dev_add_child(dev_dir, input_file))
        log_error("failed to init devices\n");
    
    return dev_dir;
}

