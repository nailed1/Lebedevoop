#include "Hiperbola.h"
#include <iostream>

using namespace std;

double Hiperbola::calculate(double x) const {
    return k / x + b;
}

void Hiperbola::print(double x) const {
    cout << "Hiperbola at x=" << x << " is " << calculate(x) << endl;
}