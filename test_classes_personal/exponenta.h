#ifndef EXPONENTA_H
#define EXPONENTA_H

#include "function.h"

class Exponenta : public Function {
private:
    double a;  // Коэффициент a
    double b;  // Показатель степени (y = a * e^(bx))
    double c;  // Смещение по y
    
public:
    Exponenta(double a = 1.0, double b = 1.0, double c = 0.0);
    
    double compute(double x) const override;
    void print() const override;
    
    double getA() const;
    double getB() const;
    double getC() const;
    
    void setA(double a);
    void setB(double b);
    void setC(double c);
};

#endif // EXPONENTA_H