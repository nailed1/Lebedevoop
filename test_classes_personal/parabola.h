#ifndef PARABOLA_H
#define PARABOLA_H

#include "function.h"

class Parabola : public Function {
private:
    double a;  // Коэффициент при x²
    double b;  // Коэффициент при x
    double c;  // Свободный член
    
public:
    Parabola(double a = 1.0, double b = 0.0, double c = 0.0);
    
    double compute(double x) const override;
    void print() const override;
    double findMin(double a, double b, double step = 0.01) const override;
    double findMax(double a, double b, double step = 0.01) const override;
    
    double getA() const;
    double getB() const;
    double getC() const;
    
    void setA(double a);
    void setB(double b);
    void setC(double c);
    
    // Аналитический поиск вершины параболы
    double getVertexX() const;
    double getVertexY() const;
};

#endif // PARABOLA_H