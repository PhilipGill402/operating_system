#include <math.h>

double sin(double x) {
    double result = 0.0f;
    
    //convert to domain [pi/2, -pi/2] because the taylor series is only accurate until around pi/2
    int quadrant = 1;
    while (!(x <= M_PI/2 && x >= -M_PI/2)){
        x -= M_PI;
        quadrant += 2;
    }
    
    quadrant %= 4;

    

    for (int i = 0; i < PRECISION; i++){
        int sign = pow(-1.0f, (double)i);
        double top = pow(x, (double)((2 * i) + 1));
        double bottom = factorial((2 * i) + 1);

        result += sign * (top / bottom);
    } 
    
    if (quadrant == 3 || quadrant == 4){
        result *= -1;
    }
    
    return result;
}

double cos(double x) {
    double result = 0.0f;
    
    //convert to domain [pi/2, -pi/2] because the taylor series is only accurate until around pi/2
    int quadrant = 1;
    while (!(x <= M_PI/2 && x >= -M_PI/2)){
        x -= M_PI;
        quadrant += 2;
    }
    
    quadrant %= 4;



    for (int i = 0; i < PRECISION; i++){
        int sign = pow(-1.0f, (double)i);
        double top = pow(x, (double)(2 * i));
        double bottom = factorial(2 * i);

        result += sign * (top / bottom);
    } 
    
    if (quadrant == 2 || quadrant == 3){
        result *= -1;
    }

    return result;
}


