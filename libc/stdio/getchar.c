#include <stdio.h>
#include <syscalls.h>

int getchar() {
    char c;
    if (read(stdin, &c, 1) != 1) return -1;

    return (int)c;
}
