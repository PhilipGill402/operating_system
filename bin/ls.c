#include <stdio.h>

int main(int argc, char* argv[]) {
    char* path;
    
    if (argc == 1) {
        //use cwd
        path = ".";
    } else if (argc > 1) {
        path = argv[1];
    }

    // if no path then use the cwd 
    uint32_t fd = open(path, O_RDONLY, 0);
    
    dirent_t* entries = malloc(sizeof(dirent_t) * 10);
    uint32_t num_entries = getdents(fd, entries, 10);
    
    for (uint32_t i = 0; i < num_entries; i++) {
        printf("%s\t", entries[i].name);
    }
    printf("\n");

    free(entries);

    close(fd);

    return 0;
}
