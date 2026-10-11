#include "fs/devfs/dev_input.h"

static uint8_t poll_input_data(dev_file_t* file, uint32_t offset) {
    (void)file;
    (void)offset;

    if (input_buffer.size)
        return POLLIN;
    else
        return 0;
} 

static int32_t read_input_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void) offset;
    
    if (!file || !buffer)
        return EFAULT;

    if (input_buffer.size == 0)
            return EAGAIN;

    uint32_t num_events = size;
    uint32_t buf_offset = 0;
    
    for (uint32_t i = 0; i < num_events; i++) {
        if (input_buffer.size == 0)
            return buf_offset;

        char c = input_buffer_read_char();

        memcpy(buffer + buf_offset, &c, sizeof(char));
        buf_offset += sizeof(char);
    }

    return buf_offset;
}

dev_file_t* create_input_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;

    file->parent = fs_node_clone(parent);
    strcpy(file->name, "input");
    file->inode = inode;
    file->get_data = read_input_data;
    file->write_data = NULL;
    file->poll_data = poll_input_data;

    return file;
}
