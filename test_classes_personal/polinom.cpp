#include "polinom.h"
#include <iostream>
#include <sstream>
#include <cmath>

Polinom::Polinom(const std::vector<double>& coeffs) 
    : Function("Полином"), coefficients(coeffs) {
    // Удаляем ведущие нули
    while (coefficients.size() > 1 && coefficients.back() == 0) {
        coefficients.pop_back();
    }
}

Polinom::Polinom(int degree) 
    : Function("Полином"), coefficients(degree + 1, 0.0) {}

double Polinom::compute(double x) const {
    // Схема Горнера для эффективного вычисления
    double result = 0.0;
    for (int i = coefficients.size() - 1; i >= 0; i--) {
        result = result * x + coefficients[i];
    }
    return result;
}

void Polinom::print() const {
    Function::print();
    std::cout << "y = ";
    
    bool first = true;
    for (int i = coefficients.size() - 1; i >= 0; i--) {
        double coeff = coefficients[i];
        
        if (coeff == 0) continue;
        
        if (!first) {
            if (coeff > 0) std::cout << " + ";
            else std::cout << " - ";
        } else {
            if (coeff < 0) std::cout << "-";
            first = false;
        }
        
        double absCoeff = std::abs(coeff);
        
        if (i == 0) {
            std::cout << absCoeff;
        } else if (i == 1) {
            if (absCoeff != 1) std::cout << absCoeff;
            std::cout << "x";
        } else {
            if (absCoeff != 1) std::cout << absCoeff;
            std::cout << "x^" << i;
        }
    }
    
    if (first) std::cout << "0";
    
    std::cout << std::endl;
}

double Polinom::getCoefficient(int degree) const {
    if (degree < 0 || degree >= static_cast<int>(coefficients.size())) {
        return 0.0;
    }
    return coefficients[degree];
}

void Polinom::setCoefficient(int degree, double value) {
    if (degree < 0) return;
    
    if (degree >= static_cast<int>(coefficients.size())) {
        coefficients.resize(degree + 1, 0.0);
    }
    
    coefficients[degree] = value;
    
    // Удаляем ведущие нули
    while (coefficients.size() > 1 && coefficients.back() == 0) {
        coefficients.pop_back();
    }
}

int Polinom::getDegree() const {
    return coefficients.size() - 1;
}

void Polinom::addCoefficient(double coeff) {
    coefficients.push_back(coeff);
}