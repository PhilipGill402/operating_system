#include <stdio.h>

#if defined(__is_libk)
#include <../../include/vga.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
    char c = (char) ic;
    terminal_write(&c, sizeof(c));
#else
    // TODO: implement system call
#endif

    return ic;
}
