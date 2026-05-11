#ifndef POLINOM_H
#define POLINOM_H

#include "function.h"
#include <vector>

class Polinom : public Function {
private:
    std::vector<double> coefficients;  // coefficients[i] - коэффициент при x^i
    
public:
    Polinom(const std::vector<double>& coeffs);
    Polinom(int degree);  // Полином заданной степени (все коэффициенты = 0)
    
    double compute(double x) const override;
    void print() const override;
    
    // Получить/установить коэффициент
    double getCoefficient(int degree) const;
    void setCoefficient(int degree, double value);
    
    // Получить степень полинома
    int getDegree() const;
    
    // Добавить коэффициент (увеличить степень)
    void addCoefficient(double coeff);
};

#endif // POLINOM_H