#ifndef STDLIB_H_
#define STDLIB_H_

#include <stdint.h>

#include <sys/cdefs.h>

__attribute__((__noreturn__))
void abort(void);

int atoi(const char* str);
long int atol(const char* str);
long long int atoll(const char* str);
double atof(const char* str);

#endif // !STDLIB_H_
