#include "exponenta.h"
#include <iostream>
#include <sstream>
#include <cmath>

Exponenta::Exponenta(double a, double b, double c) 
    : Function("Экспонента"), a(a), b(b), c(c) {}

double Exponenta::compute(double x) const {
    // y = a * e^(bx) + c
    return a * exp(b * x) + c;
}

void Exponenta::print() const {
    Function::print();
    std::cout << "y = ";
    
    if (a != 1) std::cout << a << " * ";
    std::cout << "e^(";
    if (b != 1) std::cout << b;
    std::cout << "x)";
    
    if (c >= 0) std::cout << " + " << c;
    else std::cout << " - " << std::abs(c);
    
    std::cout << std::endl;
}

double Exponenta::getA() const { return a; }
double Exponenta::getB() const { return b; }
double Exponenta::getC() const { return c; }

void Exponenta::setA(double a) { this->a = a; }
void Exponenta::setB(double b) { this->b = b; }
void Exponenta::setC(double c) { this->c = c; }