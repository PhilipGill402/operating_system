#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hello world\n");
    printf("made it\n");
    int* x = malloc(sizeof(int));
    printf("made it\n");
    printf("%x\n", x);
    *x = 10;
    printf("X: %d\n", *x);

    return 0;
}
