#include "fs/devfs/dev_input.h"

static int32_t read_input_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void) offset;

    if (!file || !buffer)
        return 0;

    if (size < sizeof(input_event_t))
        return 0;

    uint32_t num_events = size / sizeof(input_event_t);
    uint32_t buf_offset = 0;
    for (uint32_t i = 0; i < num_events; i++) {
        if (queue_empty(&input_buffer))
            break;

        input_event_t* c = (input_event_t*)dequeue(&input_buffer);
        memcpy(buffer + buf_offset, c, sizeof(input_event_t));
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

    return file;
}
