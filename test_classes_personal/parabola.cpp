#include "parabola.h"
#include <iostream>
#include <sstream>
#include <cmath>

Parabola::Parabola(double a, double b, double c) 
    : Function("Парабола"), a(a), b(b), c(c) {}

double Parabola::compute(double x) const {
    // y = ax² + bx + c
    return a * x * x + b * x + c;
}

void Parabola::print() const {
    Function::print();
    std::cout << "y = ";
    
    if (a != 0) {
        if (a != 1 && a != -1) std::cout << a;
        else if (a == -1) std::cout << "-";
        std::cout << "x²";
    }
    
    if (b != 0) {
        if (b > 0 && a != 0) std::cout << " + ";
        if (b != 1 && b != -1) std::cout << b;
        else if (b == -1) std::cout << "-";
        if (b != 0) std::cout << "x";
    }
    
    if (c != 0) {
        if (c > 0 && (a != 0 || b != 0)) std::cout << " + ";
        std::cout << c;
    }
    
    if (a == 0 && b == 0 && c == 0) std::cout << "0";
    
    std::cout << std::endl;
}

double Parabola::findMin(double start, double end, double step) const {
    if (a > 0) {
        // Парабола направлена вверх - минимум в вершине
        double vertexX = getVertexX();
        if (vertexX >= start && vertexX <= end) {
            double minY = getVertexY();
            std::cout << "Минимум функции " << name << " на [" << start << ", " << end << "]: ";
            std::cout << "f(" << vertexX << ") = " << minY << " (вершина)" << std::endl;
            return minY;
        }
    }
    
    // Иначе ищем численно
    return Function::findMin(start, end, step);
}

double Parabola::findMax(double start, double end, double step) const {
    if (a < 0) {
        // Парабола направлена вниз - максимум в вершине
        double vertexX = getVertexX();
        if (vertexX >= start && vertexX <= end) {
            double maxY = getVertexY();
            std::cout << "Максимум функции " << name << " на [" << start << ", " << end << "]: ";
            std::cout << "f(" << vertexX << ") = " << maxY << " (вершина)" << std::endl;
            return maxY;
        }
    }
    
    // Иначе ищем численно
    return Function::findMax(start, end, step);
}

double Parabola::getA() const { return a; }
double Parabola::getB() const { return b; }
double Parabola::getC() const { return c; }

void Parabola::setA(double a) { this->a = a; }
void Parabola::setB(double b) { this->b = b; }
void Parabola::setC(double c) { this->c = c; }

double Parabola::getVertexX() const {
    // x = -b/(2a)
    if (a == 0) return 0;
    return -b / (2 * a);
}

double Parabola::getVertexY() const {
    double x = getVertexX();
    return compute(x);
}