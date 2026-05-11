#include "Parabola.h"
#include <iostream>
using namespace std;

double Parabola::calculate(double x) const {
    return a * x * x + b * x + c;
}

void Parabola::print(double x) const {
    cout << "Parabola at x=" << x << " is " << calculate(x) << endl;
}