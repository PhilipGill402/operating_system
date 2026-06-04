#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUFFER_LENGTH 256

int main() {
    char buffer[MAX_BUFFER_LENGTH];
    char cwd[MAX_BUFFER_LENGTH];
    char* user_argv[10];
    
    while (1) {
        errno = 0; 
        int ret = getcwd(cwd, MAX_BUFFER_LENGTH);
        if (ret == -1) {
            perror("ret");
        }

        printf("%s >> ", cwd);
        cwd[0] = '\0';
        
        int32_t bytes = read(stdin, buffer, MAX_BUFFER_LENGTH - 1); 

        char* token = strtok(buffer, ' ');
        int argc = 0;
        
        while (token != NULL) {
            user_argv[argc++] = token;
            token = strtok(NULL, ' ');
        }

        // try to run the program, if not check the bin folder
        char* path = malloc(MAX_BUFFER_LENGTH);
        errno = 0;
        int fd = open(user_argv[0], O_RDONLY, 0);
        if (fd == -1 && errno == ENOENT) {
            strcpy(path, "/bin/"); 
            strcat(path, user_argv[0]);
        } else if (fd == -1) {
            perror("open");
            free(path);
            continue;
        } else {
            strcpy(path, user_argv[0]);
        }

        uint32_t child_pid = fork();
        if (child_pid == 0) {
            errno = 0; 
             
            int ret = execve(path, user_argv);
            
            if (ret == -1) {
                perror("execve");
                exit(-1);
            }
        }

        int status;
        waitpid(child_pid, &status, 0);

        free(path); 
    }

    return 0;
}
