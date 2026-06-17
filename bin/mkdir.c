#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Help: %s pathname\n", argv[0]);
        return -1;
    } 
    
    errno = 0;
    int32_t ret = mkdir(argv[1], 0);
    if (ret < 0) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
