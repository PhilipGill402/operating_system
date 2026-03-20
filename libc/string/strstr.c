#include <string.h>

char* strstr(char* str, char* substr){
    char* found = NULL;
    char* substr_reset = substr;
    while (*str != '\0'){
        if (*str == *substr){
            char* secondptr = str;
            while (*secondptr != '\0' && *secondptr == *substr){
                secondptr++;
                substr++;
            }
            if (*substr == '\0'){
                found = str;
                break;
            } else {
                substr = substr_reset;
            }
        }
        str++;
    }
     
    return found;
}
