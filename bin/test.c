#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/poll.h>

int main(int argc, char* argv[]) {

    int fd = open("/dev/fb", O_RDONLY, 0);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    
    pollfd_t pollfd = {
        .fd = fd,
        .events = POLLIN | POLLOUT | POLLERR,
        .revents = 0
    };

    int res = poll(&pollfd, 1);
    if (res < 0) {
        perror("poll");
        return -1;
    }

    printf("%d\n", pollfd.revents);
}
