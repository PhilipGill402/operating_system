#include <stdio.h>
#include <string.h>
#include <syscalls.h>

#define MAX_BUFFER_LENGTH 256

int main() {
    char cmd[MAX_BUFFER_LENGTH];
    char cwd[MAX_BUFFER_LENGTH];
    while (strcmp(cmd, "exit") != 0) {
        getcwd(cwd, MAX_BUFFER_LENGTH);
        printf("%s ", cwd); 
        printf(">> ");
        int bytes_read = read(stdin, cmd, MAX_BUFFER_LENGTH - 1);
        cmd[bytes_read] = '\0';
        printf("%s\n", cmd);
        cwd[0] = '\0';
    }

    return 0;
}
