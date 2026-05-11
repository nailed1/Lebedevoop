#ifndef EXPONENTA_H
#define EXPONENTA_H

#include "Function.h"

class Exponenta : public Function {
    double a, b;
public:
    Exponenta(double a, double b) : a(a), b(b) {}
    double calculate(double x) const override;
    void print(double x) const override;
};

#endif