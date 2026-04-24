#include <stdio.h>


int putchar(int ic) {
    char c = (char)ic;
    if (write(stdout, &c, 1) != 1) return -1;

    return ic;
}
