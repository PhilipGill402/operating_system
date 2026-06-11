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

    printf("FB SIZE: %u\n", info.size);
    
    uint32_t* mem = mmap(NULL, info.size, PROT_READ | PROT_WRITE, MAP_FRAMEBUFFER, -1, 0);
    
    errno = 0;
    if (munmap(mem, info.size) < 0) {
        perror("munmap");
        return -1;
    }

    return 0;
}
