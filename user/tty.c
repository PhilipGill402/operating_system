#include <stdio.h>
#include <string.h>
#include <syscalls.h>

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
    char line[MAX_BUFFER_LENGTH] = { 0 };
    char cwd[MAX_BUFFER_LENGTH] = { 0 };
    char cmd[MAX_BUFFER_LENGTH] = { 0 };
    char* args[10] = { 0 };
    while (strcmp(cmd, "exit") != 0) {
        int bytes_read = read(stdin, line, MAX_BUFFER_LENGTH - 1);
        cmd[bytes_read] = '\0';
        int argc = parse_line(line, cmd, args);
        printf("%s: ", cmd);

        for (int i = 0; i < argc; i++) {
            printf("%s, ", args[i]);
        } 
        printf("\n");

/*
        print_cmd(cmd); 
        getcwd(cwd, MAX_BUFFER_LENGTH);
        printf("%s ", cwd); 
        printf(">> ");
        int bytes_read = read(stdin, cmd, MAX_BUFFER_LENGTH - 1);
        cmd[bytes_read] = '\0';
        int ret = chdir(cmd);
        if (ret == 0) printf("failed! \n");

        cwd[0] = '\0';
*/
    }

    return 0;
}
