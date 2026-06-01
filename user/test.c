#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int fd = open("/tmp/whats_up.txt", O_CREAT | O_TRUNC, 0);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    printf("fd = %d\n", fd);

    char* buffer = "whats up world\n";
    int ret = write(fd, buffer, strlen(buffer));

    if (ret < 0) {
        perror("write");
        return -1;
    }

    printf("ret = %d\n", ret);
    
    ret = lseek(fd, 0);
    
    if (ret < 0) {
        perror("lseek");
        return -1;
    }

    char read_buffer[256];
    int bytes_read = read(fd, read_buffer, 255);

    if (bytes_read < 0) {
        perror("read");
        return -1;
    }

    printf("bytes read = %d\n", bytes_read);

    read_buffer[bytes_read] = '\0';

    printf("%s\n", read_buffer);

    close(fd);
    
    return 0;
}
