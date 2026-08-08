#include "fs/devfs/dev_input.h"

static uint8_t poll_input_data(dev_file_t* file, uint32_t offset) {
    (void)file;
    (void)offset;

    if (!queue_empty(&input_buffer))
        return POLLIN;
    else
        return 0;
} 

static int32_t read_input_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void) offset;
    
    if (!file || !buffer)
        return EFAULT;

    if (size < sizeof(input_event_t))
        return 0;

    if (queue_empty(&input_buffer))
            return EAGAIN;

    uint32_t num_events = size / sizeof(input_event_t);
    uint32_t buf_offset = 0;
    
    for (uint32_t i = 0; i < num_events; i++) {
        if (queue_empty(&input_buffer))
            return buf_offset;

        input_event_t c = *(input_event_t*)dequeue(&input_buffer);

        memcpy(buffer + buf_offset, &c, sizeof(input_event_t));
        buf_offset += sizeof(input_event_t);
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
