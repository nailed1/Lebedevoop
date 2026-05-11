#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>

class Function {
protected:
    std::string name;
    
public:
    Function(const std::string& fname);
    virtual ~Function();
    
    // Виртуальные методы
    virtual double compute(double x) const = 0;
    virtual void print() const;
    virtual double findMin(double a, double b, double step = 0.01) const;
    virtual double findMax(double a, double b, double step = 0.01) const;
    virtual double integrate(double a, double b, int n = 1000) const;
    virtual double differentiate(double x, double h = 0.0001) const;
    
    std::string getName() const;
};

#endif // FUNCTION_H