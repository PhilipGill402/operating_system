#ifndef OS_STRING_H_
#define OS_STRING_H_

#define NULL ((void*)0)

int strlen(const char* s);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);
void strcpy(char* dst, char* src);
void strncpy(char* dst, char* src, int n);
void strcat(char* dst, char* src);
void strncat(char* dst, char* src, int n);
char* strchar(char* str, int c);
char* strrchar(char* str, int c);
char* strstr(char* str, char* substr);
char* strtok(char* str, int delimiter);

#endif
