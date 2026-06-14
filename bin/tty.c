#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <gfx/gfx.h>
#include <gfx/rendering.h>
#include <gfx/8x8font.h>

#define MAX_BUFFER_LENGTH 256

uint32_t serial_fd = 0;

typedef struct {
    gfx_context_t* ctx;
    uint32_t x;
    uint32_t y;
} tty_t;

void tty_put_char(char c, tty_t* tty) {
    if (c == '\n') {
        tty->y += FONT_HEIGHT;
        tty->x = 0;
    } else if (c == '\t') {
        uint32_t col = tty->x / FONT_WIDTH;
        uint32_t next_tab_col = ((col / 4) + 1) * 4;

        while (col < next_tab_col) {
            tty_put_char(' ', tty);
            col++;
        }

    } else if (c == '\r') {
        tty->x = 0;
    } else if (c == '\b') {
        if (tty->x >= FONT_WIDTH) {
            gfx_draw_char(tty->ctx, ' ', tty->x, tty->y, FB_WHITE, FB_BLACK);
            tty->x -= FONT_WIDTH;
        } 
    } else if (c >= ' ') { 
        gfx_draw_char(tty->ctx, c, tty->x, tty->y, FB_WHITE, FB_BLACK); 
        tty->x += FONT_WIDTH;
    }

    if (tty->x >= tty->ctx->fb.width) {
        tty->x = 0;
        tty->y += FONT_HEIGHT;
    }

    if (tty->y + FONT_HEIGHT > tty->ctx->fb.height) {
        gfx_draw_rect(tty->ctx, 0, 0, tty->ctx->fb.width, tty->ctx->fb.height, FB_BLACK);
        tty->x = 0;
        tty->y = 0;
    }
}

int main() {
    tty_t tty = { 0 }; 
    serial_fd = open("/dev/serial", O_WRONLY, 0);
    uint32_t input_fd = open("/dev/input", O_RDONLY, 0);
    uint32_t ptm_fd = open("/dev/ptm", O_RDWR, 0);

    uint32_t child_pid = fork();
    if (child_pid == 0) {
        char* msg = "made it\n";
        write(serial_fd, msg, strlen(msg));

        // close all i/o file descriptors to replace with dev/pts for shell
        close(0);
        close(1);
        close(2);

        open("/dev/pts", O_RDWR, 0);
        open("/dev/pts", O_RDWR, 0);
        open("/dev/pts", O_RDWR, 0);

        errno = 0; 
         
        int ret = execve("/bin/shell", NULL);
        
        if (ret == -1) {
            exit(-1);
        }
    }

    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    tty.ctx = ctx;

    input_event_t input_event_buffer[256];
    uint8_t bytes[256];
    
     

    while (1) {
        uint32_t event_bytes_read = read(input_fd, (char*)input_event_buffer, sizeof(input_event_buffer));
        uint32_t events_read = event_bytes_read / sizeof(input_event_t);
         
        uint8_t num_chars = 0;
        for (uint32_t i = 0; i < events_read; i++) {
            input_event_t event = input_event_buffer[i];

            if (event.ch) {
                bytes[num_chars++] = event.ch; 
            }
        }

        uint32_t bytes_written = 0;
        if (num_chars > 0)
            bytes_written = write(ptm_fd, bytes, num_chars);
        
        uint32_t bytes_read = read(ptm_fd, bytes, sizeof(bytes) - 1);
        
        if (bytes_read > 0) {
            for (int32_t i = 0; i < bytes_read; i++) {
                tty_put_char((char)bytes[i], &tty);
            }

            gfx_flush(ctx);
        }

    }

    gfx_free_context(ctx);
}
