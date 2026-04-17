#include <string.h>

char* strtok(char* str, int delimiter){
    static char* string = NULL;
    if (str != NULL){
        string = str;
    }
    
    if (string == NULL){
        return NULL;
    }
    
    while (*string != '\0' && *string == (char)delimiter){
        string++;
    }
    
    if (*string == '\0'){
        string = NULL; 
        return NULL;
    }

    char* start = string;
    
    while (*string != '\0' && *string != (char)delimiter){
        string++;
    }

    if (*string){
        *string = '\0';
        string++;
    }
    
    return start;
}
