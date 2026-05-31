#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    int ret = write(256, NULL, 0);
    
    perror("write");
    return 0;
}
