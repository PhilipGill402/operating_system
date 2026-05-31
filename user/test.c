#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    int ret = write(256, NULL, 0);

    printf("%d\n", ret);

    return 0;
}
