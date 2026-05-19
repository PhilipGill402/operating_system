#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    int fd = open("/txt/hello.txt", 0);
    printf("FD: %d\n", fd);
    close(fd);
    fd = open("/txt/hello.txt", 0);
    printf("FD: %d\n", fd);
    close(fd);

    return 0;
}
