#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUFFER_LENGTH 256

void print_string(void* str) {
    printf("%r", *(string_t*)str);
}

void vector_print(const vector_t* vec, void (*print_element)(void*)) {
    printf("<"); 
    for (int i = 0; i < vector_size(vec); i++) {
        print_element(vector_get(vec, i));
        
        if (i != vector_size(vec) - 1) {
            printf(", ");
        }
    }
    printf(">\n");
}

void ps(vector_t* args) {
    (void)args;

    uint32_t fd = open("/proc/0", O_RDONLY, 0);

    char* buffer = malloc(50);
    if (!buffer) return;

    uint32_t bytes_read = read(fd, buffer, 50);

    if (bytes_read == 0) printf("no bytes read");
    else printf("%s", buffer);
   

    /*
    dirent_t* entries = malloc(sizeof(dirent_t) * 10);
    uint32_t num_entries = getdents(fd, entries, 10);

    for (uint32_t i = 0; i < num_entries; i++) {
        
    }
    */

    printf("\n");
}

void cd(vector_t* args) {
    string_t* str = (string_t*)vector_get(args, 1);
    char* path;
    if (!str)
        path = NULL;
    else
        path = string_to_literal(str);

    chdir(path);
    free(path);
    return;
}

void ls(vector_t* args) {
    string_t* vec_path = (string_t*)vector_get(args, 1);
    string_t str_path;
    if (!vec_path)
        str_path = string_literal(".");
    else 
        str_path = *vec_path;
    // if no path then use the cwd 
    char* path = string_to_literal(&str_path);
    uint32_t fd = open(path, O_RDONLY, 0);
    
    dirent_t* entries = malloc(sizeof(dirent_t) * 10);
    uint32_t num_entries = getdents(fd, entries, 10);
    
    for (uint32_t i = 0; i < num_entries; i++) {
        printf("%s\t", entries[i].name);
    }
    printf("\n");

    free(path);
}

void cat(vector_t* args) {
    string_t* str_path = (string_t*)vector_get(args, 1);
    if (!str_path) return;

    char* path = string_to_literal(str_path);

    uint32_t fd = open(path, O_RDONLY, 0);
    char buffer[MAX_BUFFER_LENGTH];
    uint32_t bytes_read = 0;
    
    do {
        bytes_read = read(fd, buffer, MAX_BUFFER_LENGTH - 1);
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }  while (bytes_read != 0);
    
    printf("\n");
    free(path);

    close(fd);
}

void run(vector_t* args) {
    string_t* str_path = (string_t*)vector_get(args, 1);

    if (!str_path) return;
    
    int argc = vector_size(args) - 1;
    char* argv[argc + 1];
    
    for (int i = 0; i < argc; i++) {
        string_t arg = *(string_t*)vector_get(args, i + 1);
        argv[i] = string_to_literal(&arg);
    }

    argv[argc] = NULL;
    
    char* path = string_to_literal(str_path);

    uint32_t pid = fork();
    
    if (pid == 0) {
        errno = 0; 
        int ret = execve(path, argv);
        if (ret == -1) {
            perror("execve");
        }
    }
    
    int status;
    waitpid(pid, &status, 0);

    free(path);
}

void print_str(void* str) {
    printf("%r", *(string_t*)str);
}

int main(int argc, char* argv[]) {
    string_t line = string_create();
    char buffer[MAX_BUFFER_LENGTH];
    char cwd[MAX_BUFFER_LENGTH];
    
    while (1) {
        getcwd(cwd, MAX_BUFFER_LENGTH); 
        printf("%s >> ", cwd);
        cwd[0] = '\0';
        
        uint32_t bytes = read(stdin, buffer, MAX_BUFFER_LENGTH - 1);
        buffer[bytes] = '\0';

        line = string_literal(buffer);
        vector_t args = string_tokenize(&line, ' ');
        string_t cmd = *(string_t*)vector_get(&args, 0);
        
        if (string_compare_literal(&cmd, "cd") == 0) {
            cd(&args); 
        } else if (string_compare_literal(&cmd, "ls") == 0) {
            ls(&args);
        } else if (string_compare_literal(&cmd, "exit") == 0) {
            break;
        } else if (string_compare_literal(&cmd, "run") == 0) {
            run(&args);
        } else if (string_compare_literal(&cmd, "cat") == 0) {
            cat(&args);
        } else if (string_compare_literal(&cmd, "ps") == 0) {
            ps(&args);
        } else {
            printf("Couldn't recognize '%r' command\n", cmd);
        }
    
        for (uint32_t i = 0; i < vector_size(&args); i++) {
            string_t str = *(string_t*)vector_get(&args, i);
            string_free(&str);
        }
    }

    

    return 0;
}
