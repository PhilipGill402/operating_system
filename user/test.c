#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* num = "123.456 abc 456";
    double converted = atof(num);

    printf("str: %s\nconverted: %f\n", num, converted);

    return 0;
}
