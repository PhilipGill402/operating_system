#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    uint32_t serial_fd = open("/dev/serial", O_WRONLY, 0);
    printf("SERIAL: %d", serial_fd);
    write(serial_fd, "hello", 6); 
    uint32_t input_fd = open("/dev/input", O_RDONLY, 0);
    uint32_t ptm_fd = open("/dev/ptm", O_RDWR, 0);

    uint32_t child_pid = fork();
    if (child_pid == 0) {
        // close all i/o file descriptors to replace with dev/pts for shell
        int pts = open("/dev/pts", O_RDWR, 0);

        dup2(pts, 0); 
        dup2(pts, 1);
        dup2(pts, 2);

        errno = 0; 
        int ret = execve("/bin/shell", NULL);
        
        if (ret == -1) {
            exit(-1);
        }
    }

    char input_buffer[256];
    uint8_t bytes[256];
    
    pollfd_t polls[2];
    // Input 
    polls[0].fd = input_fd;
    polls[0].events = POLLIN;
    // PTM 
    polls[1].fd = ptm_fd;
    polls[1].events = POLLIN;

    while (1) {
        polls[0].revents = 0;
        polls[1].revents = 0;
        int32_t ready = poll(polls, 2);

        if (ready <= 0) {
            yield();
            continue;
        }
        
        if (polls[0].revents & POLLIN) {
            uint32_t bytes_read = read(input_fd, (char*)input_buffer, sizeof(input_buffer));
            
            uint8_t num_chars = 0;
            for (uint32_t i = 0; i < bytes_read; i++) {
                bytes[num_chars++] = input_buffer[i];
            }

            uint32_t bytes_written = 0;
            if (num_chars > 0) {
                bytes_written = write(ptm_fd, bytes, num_chars);
            }
        }
        
        if (polls[1].revents & POLLIN) {
            uint32_t bytes_read = read(ptm_fd, bytes, sizeof(bytes) - 1);
            
            if (bytes_read > 0) {
                for (int32_t i = 0; i < bytes_read; i++) {
                    printf("%c", (char)bytes[i]);
                }
            }
        }
        
    }
}
