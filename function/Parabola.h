#ifndef PARABOLA_H
#define PARABOLA_H

#include "Function.h"

class Parabola : public Function {
    double a, b, c;
public:
    Parabola(double a, double b, double c) : a(a), b(b), c(c) {}
    double calculate(double x) const override;
    void print(double x) const override;
};
#endif