#include <math.h>

double clamp(double x, double min, double max) {
    if (x < min){
        x = min;
    } else if (x > max){
        x = max;
    }

    return x;
}

double fmin(int n, ...) {
    va_list args;
    va_start(args, n);
    
    double min = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double curr = va_arg(args, double);
        if (curr < min) min = curr;
    }
    va_end(args);

    return min;
}

double fmax(int n, ...) {
    va_list args;
    va_start(args, n);
    
    double max = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double curr = va_arg(args, double);
        if (curr > max) max = curr;
    }
    va_end(args);

    return max;
}

double ceil(double x) {
    int i = (int)x;

    if (x > i) {
        return (double)(i + 1);
    }

    return (double)i;
}

double floor(double x) {
    int i = (int)x;

    if (x < i) {
        return (double)(i - 1);
    }

    return (double)i;
}

double trunc(double x) {
    (double)((int)x);
}


