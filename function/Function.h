#ifndef FUNCTION_H
#define FUNCTION_H

class Function{

public:
    virtual ~Function() {}
    
    virtual double calculate(double x) const = 0;
    virtual void print(double x) const = 0;

    virtual double findMin(double a, double b, double h = 0.01) const;
    virtual double findMax(double a, double b, double h = 0.01) const;

    virtual double differential(double x, double h = 0.0001) const;
    virtual double integrate(double a, double b, int n = 1000) const;
};

#endif