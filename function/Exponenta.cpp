#include "Exponenta.h"
#include <cmath>
#include <iostream>

using namespace std;

double Exponenta::calculate(double x) const {
    return a * exp(b * x);
}

void Exponenta::print(double x) const {
    cout << "Exponenta at x=" << x << " is " << calculate(x) << endl;
}