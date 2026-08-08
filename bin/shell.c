#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_BUFFER_LENGTH 256

uint32_t serial_fd;

static void str_free(void* element) {
    char* str = *(char**)element;
    free(str);
}

static void str_print(void* str) {
    printf(*(char**)str);
}

static int read_line(char* buffer, uint32_t max_len) {
    uint32_t len = 0;

    while (len < max_len - 1) {
        char c;
        int32_t n = read(0, &c, 1);
        if (n <= 0)
            continue;

        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            buffer[len] = '\0';
            printf("\n");
            return len;
        }

        if (c == '\b' || c == 0x7F) {
            if (len > 0) {
                len--;
                printf("\b \b");
            }
            continue;
        }
        
        buffer[len++] = c;
        write(1, &c, 1); // echo typed character
    }

    buffer[len] = '\0';
    return len;
}

int check_builtins(int argc, char* argv[]) {
    if (argc == 0)
        return 1;

    if (strcmp(argv[0], "cd") == 0) {
        if (argc >= 2)
            chdir(argv[1]);
        else 
            printf("'cd' requires an argument\n");

        return 1;
    } else if (strcmp(argv[0], "exit") == 0) {
        if (argc >= 2)
            exit(atoi(argv[1]));
        else
            exit(0);
    }

    return 0;
}

int main() {
    serial_fd = open("/dev/serial", O_WRONLY, 0);
    char buffer[MAX_BUFFER_LENGTH];
    char cwd[MAX_BUFFER_LENGTH];
    
    while (1) {
        char* user_argv[10] = { 0 };
        errno = 0; 
        int ret = getcwd(cwd, MAX_BUFFER_LENGTH);
        if (ret == -1) {
            perror("getcwd");
        }

        printf("%s >> ", cwd);
        cwd[0] = '\0';
        
        int32_t bytes = read_line(buffer, MAX_BUFFER_LENGTH - 1);

        buffer[bytes] = '\0';

        vector_t tokens = strtok(buffer, ' ');
        int user_argc = vector_size(&tokens);
        
        for (int i = 0; i < user_argc; i++)
            user_argv[i] = *(char**)vector_get(&tokens, i);

        // if command is a builtin one then skip
        if (check_builtins(user_argc, user_argv)) {
            continue; 
        }

        // try to run the program, if not check the bin folder
        char* path = malloc(MAX_BUFFER_LENGTH);
        memset(path, 0, MAX_BUFFER_LENGTH);
        
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
            close(fd);
            strcpy(path, user_argv[0]);
        }
        
        uint32_t child_pid = fork();
        if (child_pid == 0) {
            errno = 0; 
             
            int ret = execve(path, user_argv);
            
            if (ret == -1) {
                perror("execve");
                printf("%s\n", path);
                exit(-1);
            }
        }

        int status;
        waitpid(child_pid, &status, 0);
        
        free(path);
        vector_free_destructor(&tokens, str_free);
    }

    return 0;
}
