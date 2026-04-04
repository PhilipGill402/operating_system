#include "tty/tty.h"

static input_buffer_t keyboard_buffer;

void input_buffer_reset() {
    keyboard_buffer.data[0] = '\0';
    keyboard_buffer.length = 0;
    keyboard_buffer.ready = 0;
}

void input_buffer_push(char c) {
    if (keyboard_buffer.length + 1 >= INPUT_BUFFER_SIZE) {
        return;
    }

    keyboard_buffer.data[keyboard_buffer.length++] = c;
    keyboard_buffer.data[keyboard_buffer.length] = '\0';
}
void input_buffer_pop() {
    if (keyboard_buffer.length == 0) {
        return;
    }

    keyboard_buffer.data[--keyboard_buffer.length] = '\0';
}

void input_buffer_submit() {
    keyboard_buffer.data[keyboard_buffer.length] = '\0';
    keyboard_buffer.ready = 1;
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
    
    //check bin/ for the command
    fs_node_t* bin = resolve_path_from(fs_root, "bin");
    fs_node_t* elf = resolve_path_from(bin, line);

    if (!elf) {
        return;
    } 
    
    elf_execute(elf); 
}

void tty() {
    input_buffer_reset();
    char cmd[INPUT_BUFFER_SIZE] = "";

    while (strcmp(cmd, "exit") != 0) {
        printf(">> ");

        while (!keyboard_buffer.ready) {
            __asm__ __volatile__("hlt");
        }
        
        strcpy(cmd, keyboard_buffer.data);
        read_command(cmd); 
    
        input_buffer_reset();
    } 
}
