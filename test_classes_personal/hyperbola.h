#ifndef HYPERBOLA_H
#define HYPERBOLA_H

#include "function.h"

class Hyperbola : public Function {
private:
    double a;  // Коэффициент a
    double b;  // Коэффициент b (смещение по x)
    double c;  // Коэффициент c (смещение по y)
    
public:
    Hyperbola(double a = 1.0, double b = 0.0, double c = 0.0);
    
    double compute(double x) const override;
    void print() const override;
    
    double getA() const;
    double getB() const;
    double getC() const;
    
    void setA(double a);
    void setB(double b);
    void setC(double c);
};

#endif // HYPERBOLA_H