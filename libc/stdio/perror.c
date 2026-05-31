#include <stdio.h>

void perror(const char* str) {
    if (str && *str != '\0') {
        printf("%s: ", str);
    }

    printf("%s\n", strerror(errno));
}
