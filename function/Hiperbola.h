#ifndef HIPERBOLA_H
#define HIPERBOLA_H

#include "Function.h"

class Hiperbola: public Function{
    double k, b;
public:
    Hiperbola(double k, double b) : k(k), b(b) {}
    double calculate(double x) const override;
    void print(double x) const override;
};

#endif