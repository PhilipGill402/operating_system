#include <stdio.h>

#define MAX_BUFFER_LENGTH 256

int main(int argc, char* argv[]) {
    if (argc != 2)
        return -1;

    char* path = argv[1];

    uint32_t fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buffer[MAX_BUFFER_LENGTH];
    uint32_t bytes_read = 0;
    
    do {
        bytes_read = read(fd, buffer, MAX_BUFFER_LENGTH - 1);
        if (bytes_read == -1) {
            perror("read");
            return -1;
        }

        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }  while (bytes_read != 0);
    
    printf("\n");
    free(path);

    close(fd);
}
