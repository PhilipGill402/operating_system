#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>

#define MAX_BUFFER_LENGTH 256

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

/*
 * init buffers
 * read line
 * parse command
 * parse args
 * run corresponding function
 * repeat
 *
 */

void print_str(void* str) {
    printf("%r", *(string_t*)str);
}

int main() {
    string_t line = string_create();
    char buffer[MAX_BUFFER_LENGTH];
    char cwd[MAX_BUFFER_LENGTH];
    
    while (1) {
        getcwd(cwd, MAX_BUFFER_LENGTH); 
        printf("%s >> ", cwd);
        cwd[0] = '\0';
        
        read(stdin, buffer, MAX_BUFFER_LENGTH - 1);

        line = string_literal(buffer);
        vector_t args = string_tokenize(&line, ' ');
        string_t cmd = *(string_t*)vector_get(&args, 0);
        
        if (string_compare_literal(&cmd, "cd") == 0) {
            cd(&args); 
        } else if (string_compare_literal(&cmd, "exit") == 0) {
            break;
        }
    
        for (uint32_t i = 0; i < vector_size(&args); i++) {
            string_t str = *(string_t*)vector_get(&args, i);
            string_free(&str);
        }
    }

    

    return 0;
}
