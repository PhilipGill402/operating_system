#include "fs/devfs/dev_fb.h"

int32_t get_fb_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    if (!file || !buffer)
        return 0;
    
    uint32_t fb_len = framebuffer.width * framebuffer.height * framebuffer.pitch;
    if (offset >= fb_len)
        return 0;
    
    uint32_t bytes_to_read = offset + size < fb_len ? offset + size : fb_len - offset;
    uint8_t* addr = framebuffer.addr + offset;
    
    memcpy(buffer, addr, size);

    return bytes_to_read;
}

int32_t write_fb_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    if (!file || !buffer)
        return 0;
    
    uint32_t fb_len = framebuffer.height * framebuffer.pitch;
    if (offset >= fb_len)
        return 0;
    
    uint32_t bytes_to_write = offset + size < fb_len ? size : fb_len - offset;
    uint8_t* addr = framebuffer.addr + offset;
    
    memcpy(addr, buffer, bytes_to_write);

    return bytes_to_write;
}

dev_file_t* create_fb_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;
    
    file->parent = fs_node_clone(parent);
    strcpy(file->name, "fb");
    file->inode = inode;
    file->get_data = get_fb_data;
    file->write_data = write_fb_data;

    return file;
}
