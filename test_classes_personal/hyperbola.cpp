#include "hyperbola.h"
#include <iostream>
#include <sstream>
#include <cmath>

Hyperbola::Hyperbola(double a, double b, double c) 
    : Function("Hyperbola"), a(a), b(b), c(c) {}

double Hyperbola::compute(double x) const {
    // y = a/(x+b) + c
    if (std::abs(x + b) < 1e-10) {
        return std::numeric_limits<double>::infinity();
    }
    return a / (x + b) + c;
}

void Hyperbola::print() const {
    Function::print();
    std::cout << "y = " << a << "/(x";
    if (b >= 0) std::cout << " + " << b;
    else std::cout << " - " << std::abs(b);
    std::cout << ")";
    if (c >= 0) std::cout << " + " << c;
    else std::cout << " - " << std::abs(c);
    std::cout << std::endl;
}

double Hyperbola::getA() const { return a; }
double Hyperbola::getB() const { return b; }
double Hyperbola::getC() const { return c; }

void Hyperbola::setA(double a) { this->a = a; }
void Hyperbola::setB(double b) { this->b = b; }
void Hyperbola::setC(double c) { this->c = c; }