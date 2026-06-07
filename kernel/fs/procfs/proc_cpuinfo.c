#include "fs/procfs/proc_cpuinfo.h"

static int32_t get_cpuinfo_data(proc_file_t* proc, uint8_t* buffer, uint32_t offset, uint32_t size) {
    char* vendor = cpu_get_vendor();
    char* brand = cpu_get_brand();
    log_debug("CPU: %s\n", vendor);
    log_debug("CPU: %s\n", brand);
    
    char* fmt =
        "vendor_id\t: %s\n"
        "model name\t: %s\n";

    char intern_buffer[64];
    uint32_t bytes_written = snprintf(intern_buffer, 64, fmt, vendor, brand);
    intern_buffer[bytes_written] = '\0';

    uint32_t len = strlen(intern_buffer);

    if (offset > len)
        return 0;

    bytes_written = offset + size < len ? offset + size : len - offset;
    
    snprintf(buffer, bytes_written, fmt, vendor, brand);

    kfree(vendor);
    kfree(brand);
    return bytes_written;
}

proc_file_t* create_cpuinfo_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    proc_file_t* file = kmalloc(sizeof(proc_file_t));
    if (!file)
        return NULL;


    file->parent = fs_node_clone(parent);
    strcpy(file->name, "cpuinfo");
    file->inode = inode;
    file->get_data = get_cpuinfo_data;
    
    return file;
}
