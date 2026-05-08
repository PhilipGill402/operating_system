#include <math.h>

int powi(int x, int y) {
    if (n == 0) {
        return 1;
    }

    int negative = n < 0;

    if (negative) {
        n = -n;
    }

    int result = 1;
    int base = x;

    while (n > 0) {
        if (n & 1) {
            result *= base;
        }

        base *= base;
        n >>= 1;
    }

    return negative ? 1.0 / result : result;
}

int factorial(int x){
    if (x == 0){
        return 1;
    }
    
    int result = 1;

    for (int i = x; i > 0; i--){
        result *= i; 
    }

    return result;
}

double abs(double x){
    if (x < 0){
        return x *= -1;
    }

    return x;
}

double sqrt(double x){
    double next_guess;
    double prev_guess;

    if (x >= 1){
        prev_guess = x / 2.0;
    } else if (x > 0 && x < 1){
        prev_guess = 1.0;
    } else if (x == 0){
        return 0;
    } else {
        //can't find square root of a negative number
        return -1;
    }
    
    do{
        next_guess = prev_guess - (.5 * (prev_guess - (x / prev_guess)));
        prev_guess = next_guess;
    } while (d_abs((next_guess*next_guess) - x) > EPSILON);

    return next_guess;
}

double exp(double x) {
    double ln2 = 0.6931471805599453;

    if (x == 0.0) {
        return 1.0;
    }

    int k = (int)(x / ln2);
    double r = x - k * ln2;

    double term = 1.0;
    double sum = 1.0;

    for (int n = 1; n <= 20; n++) {
        term *= r / n;
        sum += term;
    }

    while (k > 0) {
        sum *= 2.0;
        k--;
    }

    while (k < 0) {
        sum *= 0.5;
        k++;
    }

    return sum;
}

double exp2(double x) {
    double ln2 = 0.6931471805599453;
    int k = (int)x;
    double r = x - k;
    
    if (x < 0.0 && r != 0.0) {
        k -= 1;
        r = x - k;
    }

    double y = r * ln2;

    double term = 1.0;
    double sum = 1.0;

    for (int n = 1; n <= 20; n++) {
        term *= y / n;
        sum += term;
    }

    while (k > 0) {
        sum *= 2.0;
        k--;
    }

    while (k < 0) {
        sum *= 0.5;
        k++;
    }

    return sum;
}


