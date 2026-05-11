#include "Function.h"
#include "Exponenta.h"
#include "Parabola.h"
#include "Hiperbola.h"
#include <iostream>
#include <vector>

using namespace std;

int main(){
    vector<Function*> functions;

    functions.push_back(new Exponenta(1.0, 0.5)); // a=1, b=0.5
    functions.push_back(new Parabola(1.0, -2.0, 1.0)); // x^2 - 2x + 1
    functions.push_back(new Hiperbola(2.0, 1.0)); // k=2, b=1

    for (Function* f : functions) {
        f->print(2.0);
        
        cout << "Min on [0, 5]: " << f->findMin(0, 5) << endl;
        cout << "Max on [0, 5]: " << f->findMax(0, 5) << endl;
        cout << "Integral [0, 1]: " << f->integrate(0, 1) << endl;
        cout << "Differential at 1.0: " << f->differential(1.0) << endl;
    }

    for (Function* f : functions) {
        delete f;
    }

    return 0;
}