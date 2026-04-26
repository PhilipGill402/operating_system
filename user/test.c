#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hello world\n");
    int* x = malloc(sizeof(int) * 4096);
    
    for (int i = 0; i < 4096; i++) {
        x[i] = i;
    }

    for (int i = 0; i < 4096; i++) {
        printf("%d\n", x[i]);
    } 

    return 0;
}
