#include "Function.h"
#include <iostream>

using namespace std;

double Function::findMin(double a, double b, double h) const {
    double minVal = calculate(a);
    for (double x = a + h; x <= b; x += h) {
        double val = calculate(x);
        minVal = (val < minVal) ? val : minVal;
    }
    return minVal;
}

double Function::findMax(double a, double b, double h) const {
    double maxVal = calculate(a);
    for (double x = a + h; x <= b; x += h){
        double val = calculate(x);
        maxVal = (val > maxVal) ? val : maxVal;
    }
    return maxVal;
}

// метод центральной разности
double Function::differential(double x, double h) const{
    return (calculate(x + h) - calculate(x - h)) / (2 * h);
}

// метод трапеций
double Function::integrate(double a, double b, int n) const{
    double h = (b - a) / n;
    double sum = 0.5 * (calculate(a) + calculate(b));
    for (int i = 1; i < n; i++) {
        sum += calculate(a + i * h);
    }
    return sum * h;
}