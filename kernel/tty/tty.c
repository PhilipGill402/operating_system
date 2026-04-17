#include "tty/tty.h"

void print_cwd() {
    fs_node_t* start = fs_cwd;
    fs_node_t* path[10] = { start };
    uint8_t idx = 1;

    while (start->inode != fs_root->inode) {
        start = fs_parent(start);

        path[idx++] = start;
    }

    for (int i = idx - 1; i >= 0; i--) {
        printf("%s", path[i]->name);
    }
}



void read_command(char* line) {
    char* cmd = strtok(line, ' ');
     
    //check command list
    for (uint8_t i = 0; i < num_commands; i++) {
        if (strcmp(cmd, commands[i].cmd) == 0) {
            commands[i].handler(cmd);
            return;
        }
    }
    
    
    fs_node_t* elf = resolve_path_from(fs_cwd, line);
    
    //check bin/ for the command if not found in current directory
    if (!elf) {
        fs_node_t* bin = resolve_path_from(fs_root, "bin");
        elf = resolve_path_from(bin, line);
    }

    if (!elf) {
        return;
    } 
    
    elf_execute(elf); 
}

void tty() {
    input_buffer_reset();
    char cmd[INPUT_BUFFER_SIZE] = "";
    while (strcmp(cmd, "exit") != 0) {
        print_cwd(); 
        printf(" >> ");

        while (!input_buffer_ready()) {
            __asm__ __volatile__("hlt");
        }
        
        strcpy(cmd, input_buffer_data());
        read_command(cmd); 
        input_buffer_reset();
    } 
}
