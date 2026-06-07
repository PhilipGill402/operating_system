#include "fs/procfs/proc_meminfo.h"

static inline uint64_t frame_to_kb(uint32_t frame) {
    return ((uint64_t)frame * PAGE_SIZE) / 1024;
}

static int32_t get_meminfo_data(proc_file_t* proc, uint8_t* buffer, uint32_t offset, uint32_t size) {
    char* fmt =
        "Total Memory:\t%l kB\n"
        "Free Memory:\t%l kB\n"
        "Used Memory:\t%l kB\n"
        "Page Size:\t%d kB\n";
    
    uint64_t total_kb = frame_to_kb(pmm.total_frames);
    uint64_t used_kb = frame_to_kb(pmm.used_frames);
    uint64_t free_kb = frame_to_kb(pmm.free_frames);
     
    char intern_buffer[256];
    uint32_t bytes_written = snprintf(intern_buffer, 256, fmt, total_kb, free_kb, used_kb, 4);
    intern_buffer[bytes_written] = '\0';

    uint32_t len = strlen(intern_buffer);

    if (offset > len)
        return 0;

    bytes_written = offset + size < len ? offset + size : len - offset;
    
    snprintf(buffer, bytes_written, fmt, total_kb, free_kb, used_kb, 4);

    return bytes_written;
}

proc_file_t* create_meminfo_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    proc_file_t* file = kzmalloc(sizeof(proc_file_t));
    if (!file)
        return NULL;

    file->parent = fs_node_clone(parent);
    strcpy(file->name, "meminfo");
    file->inode = inode;
    file->get_data = get_meminfo_data;

    return file;
}
