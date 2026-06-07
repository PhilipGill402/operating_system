#include <stdio.h>
#include <unistd.h>
#include <ctype.h>


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    uint32_t fd = open("/proc", O_RDONLY, 0);
    
    dirent_t* entries = malloc(sizeof(dirent_t) * 10);
    uint32_t num_entries = getdents(fd, entries, 10);

    for (uint32_t i = 0; i < num_entries; i++) {
        char* buffer = malloc(256);
        if (!buffer) 
            return -1;
        
        char* file = malloc(64);
        strcpy(file, "/proc/");
        
        // make sure we only get processes
        if (isdigit(entries[i].name[0])) {
            continue;
        }

        strcat(file, entries[i].name);
        
        uint32_t proc_fd = open(file, O_RDONLY, 0);
        if (proc_fd == -1) {
            perror("open");
            return -1;
        }

        uint32_t bytes_read = read(proc_fd, buffer, 256);
        buffer[bytes_read] = '\0';

        if (bytes_read == 0)
            printf("no bytes read\n");
        else
            printf("%s\n", buffer);
        
        memset(buffer, 0, 256);
        memset(file, 0, 64);
        free(buffer);
        free(file);
    }

    free(entries);

    return 0;
}

