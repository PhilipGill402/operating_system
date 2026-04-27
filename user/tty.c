#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <unistd.h>

#define MAX_BUFFER_LENGTH 256

void ch(const char* path) {
    return;
}

void print_cmd(char* cmd) {
    printf("CMD: "); 
    while (*cmd != '\0') {
        printf("%d, ", (int)*cmd++);
    }
    printf("\n\n");
}

int parse_line(char* line, char* cmd, char** args) {
    int count = 0;
    char* tokens[11];
    
    char* token = strtok(line, ' ');

    while (token != NULL) {
        printf("%x\n", token);
        strcpy(tokens[count++], token);
        token = strtok(NULL, ' ');
        printf("ARG: %s\n", args[count - 1]);
    }

    return --count;
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

int main() {
    string_t cmd = string_literal("test");
    vector_t args = vector_create(sizeof(string_t));
    char* buffer[MAX_BUFFER_LENGTH];
    
    do {
        string_free(&cmd); 
        printf(">> ");
        
        read(stdin, buffer, MAX_BUFFER_LENGTH - 1);

        cmd = string_literal(buffer);
        
        if (string_compare_literal(&cmd, "cd") == 0) {
            printf("change dir!\n");
        }

    } while (string_compare_literal(&cmd, "exit") != 0);

    return 0;
}
