#include "function.h"
#include <iostream>
#include <cmath>
#include <limits>

Function::Function(const std::string& fname) : name(fname) {}

Function::~Function() {}

void Function::print() const {
    std::cout << "Функция: " << name << std::endl;
}

double Function::findMin(double a, double b, double step) const {
    double minVal = compute(a);
    double minX = a;
    
    for (double x = a; x <= b; x += step) {
        double val = compute(x);
        if (val < minVal) {
            minVal = val;
            minX = x;
        }
    }
    
    std::cout << "Минимум функции " << name << " на [" << a << ", " << b << "]: ";
    std::cout << "f(" << minX << ") = " << minVal << std::endl;
    
    return minVal;
}

double Function::findMax(double a, double b, double step) const {
    double maxVal = compute(a);
    double maxX = a;
    
    for (double x = a; x <= b; x += step) {
        double val = compute(x);
        if (val > maxVal) {
            maxVal = val;
            maxX = x;
        }
    }
    
    std::cout << "Максимум функции " << name << " на [" << a << ", " << b << "]: ";
    std::cout << "f(" << maxX << ") = " << maxVal << std::endl;
    
    return maxVal;
}

double Function::integrate(double a, double b, int n) const {
    // Метод трапеций
    double h = (b - a) / n;
    double sum = (compute(a) + compute(b)) / 2.0;
    
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += compute(x);
    }
    
    double result = sum * h;
    std::cout << "Интеграл функции " << name << " на [" << a << ", " << b << "]: ";
    std::cout << result << std::endl;
    
    return result;
}

double Function::differentiate(double x, double h) const {
    // Численное дифференцирование (центральная разность)
    double result = (compute(x + h) - compute(x - h)) / (2 * h);
    
    std::cout << "Производная функции " << name << " в точке x = " << x << ": ";
    std::cout << result << std::endl;
    
    return result;
}

std::string Function::getName() const {
    return name;
}