#include <stdlib.h>

#include <stdio.h>

static inline int is_char_num(char c) {
    return c >= 48 && c <= 57;
}

int atoi(const char* str) {
    int num = 0; 

    for (uint32_t i = 0; str[i] != '\0' && is_char_num(str[i]); i++) {
        num = num * 10 + (str[i] - '0'); 
    }

    return num;
}

long int atol(const char* str) {
    long int num = 0; 

    for (uint32_t i = 0; str[i] != '\0' && is_char_num(str[i]); i++) {
        num = num * 10 + (str[i] - '0'); 
    }

    return num;
}

long long int atoll(const char* str) {
    long long int num = 0; 

    for (uint32_t i = 0; str[i] != '\0' && is_char_num(str[i]); i++) {
        num = num * 10 + (str[i] - '0'); 
    }

    return num;
}

double atof(const char* str) {
    double num = 0;

    uint32_t idx; 
    for (idx = 0; str[idx] != '\0' && is_char_num(str[idx]) && str[idx] != '.'; idx++) {
        num = num * 10 + (str[idx] - '0');
    }

    if (str[idx] == '.') {
        double base = 1;
        idx++;

        for (; str[idx] != '\0' && is_char_num(str[idx]); idx++) {
            base /= 10;
            num += base * (str[idx] - '0');
        }
    }

    return num;
}
