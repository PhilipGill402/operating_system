#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hello world\n");
    
    int* x = malloc(sizeof(int));
    printf("%x\n", x);
    *x = 10;
    printf("X: %d\n", *x);

    return 0;
}
