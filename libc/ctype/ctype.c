#include <ctype.h>

int isalnum(int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

int isalpha(int c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

int isblank(int c) {
    return (c == 32) || (c == 9);
}

int iscntrl(int c) {
    return (c >= 0 && c <= 31) || (c == 127);
}

int isdigit(int c) {
    return c >= 48 && c <= 57;
}

int isgraph(int c) {
    return c >= 33 && c <= 126;
}

int islower(int c) {
    return c >= 97 && c <= 122;
}

int isprint(int c) {
    return c >= 32 && c <= 126;
}

int ispunct(int c) {
    return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
}

int isspace(int c) {
    return (c == 32) || (c >= 9 && c <= 13);
}

int isupper(int c) {
    return (c >= 65 && c <= 90);
}

int isxdigit(int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102);
}

int tolower(int c) {
    if (c >= 65 && c <= 90)
        return 97 + (c - 65);
    else
        return c;
}

int toupper(int c) {
    if (c >= 97 && c <= 122)
        return 65 + (c - 97);
    else 
        return c;
}


