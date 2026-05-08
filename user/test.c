#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    uint32_t fd = open("/txt/hello.txt", 0);
    
    uint32_t written = fprintf(fd, "hello %s times %d", "world", 10);

    
    lseek(fd, 0);
    char* buffer = malloc(50);
    uint32_t bytes_read = read(fd, buffer, 50);
    buffer[bytes_read] = '\0';
    

    printf("%s\n", buffer); 

    close(fd);
    return 0;
}
