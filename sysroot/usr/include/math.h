#ifndef MATH_H_
#define MATH_H_

#include <stdint.h>
#include <stdarg.h>

#define EPSILON 1e-12
#define PRECISION 1e-12
#define M_PI 3.14

// trig functions
double sin(double x);
double cos(double x);

// algebra functions
int powi(int x, int y);
double abs(double x);
double sqrt(double x);
double exp(double x);
double exp2(double x);

// miscellaneous
int factorial(int x);
double clamp(double x, double min, double max);
double fmin(int n, ...);
double fmax(int n, ...);
double ceil(double x);
double floor(double x);
double trunc(double x);

#endif
