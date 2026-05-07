#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("Hello world\n");
    
    string_t str = string_create();

    sprintf(&str, "Hello %s times %d\n", "world", 10);

    printf("%r\n", str);

    return 0;
}
