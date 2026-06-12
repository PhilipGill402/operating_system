#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

int main(void) {
    fb_info_t info;
    
    errno = 0;
    if (!fb_info(&info)) {
        perror("fb_info");
        return -1;
    }

    uint32_t* fb = mmap(NULL, info.size, PROT_READ | PROT_WRITE, MAP_FRAMEBUFFER, -1, 0);
    if (!fb) {
        printf("mmap failed\n");
        return -1;
    }
    
    errno = 0;
    if (munmap(fb, info.size) < 0) {
        perror("munmap");
        return -1;
    }

    return 0;
}
