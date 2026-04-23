#include "rational.h"
#include <iostream>
#include <cmath>

using namespace std;

Rational my_abs(Rational r) {
    return (r < Rational(0)) ? Rational(-r.num, r.den) : r;
}

static long long gcd(long long a, long long b) {
    while (b) { 
        long long t = b; 
        b = a % b; 
        a = t; 
    }
    return a;
}

// чцелочисленный корень
long long integer_sqrt(long long N) {
    if (N < 0) throw std::invalid_argument("Negative radicand");
    if (N == 0 || N == 1) return N;
    
    long long x = N;
    long long y = (x + N / x) / 2;
    
    while (y < x) {
        x = y;
        y = (x + N / x) / 2;
    }
    return x;
}

Rational sqrt_rational(const Rational& D) {
    if (D < Rational(0))
        throw std::invalid_argument("Negative radicand");
    if (D == Rational(0))
        return Rational(0);
    
    long long a = D.num;
    long long b = D.den;
    
    // Упрощаем дробь для избежания переполнения
    long long g = gcd(abs(a), abs(b));
    a /= g;
    b /= g;
    
    long long N = a * b;
    long long root = integer_sqrt(N);
    
    // Проверяем точность
    Rational approx(root, b);
    Rational diff = approx * approx - D;
    if (diff < Rational(0)) 
        diff = Rational(-diff.num, diff.den);
    return approx;
}

struct Roots {
    bool real;
    Rational x1, x2;
};

Roots solve_quadratic(const Rational& a, const Rational& b, const Rational& c) {
    if (a == Rational(0)) throw std::invalid_argument("Not a quadratic equation");
    
    Rational D = b * b - Rational(4) * a * c;
    cout << "D = " << D.num << "/" << D.den << " = " << (double)D << endl;
    if (D < Rational(0)) {
        return {false, Rational(), Rational()};
    }
    
    // Rational eps(1, 1000000);
    Rational sqrtD = sqrt_rational(D);
    
    Rational two_a = Rational(2) * a;
    Rational minus_b = b * Rational(-1);
    Rational x1 = (minus_b - sqrtD) / two_a;
    Rational x2 = (minus_b + sqrtD) / two_a;
    
    return {true, x1, x2};
}

int main(){
    Rational a(1);
    Rational b(567, 345);
    Rational c(2, 51);
    Roots res = solve_quadratic(a, b, c);
    cout << res.x1.num << "/" << res.x1.den << "\n" << res.x2.num << "/" << res.x2.den << endl;
    cout << (double)res.x1 << "\n" << (double)res.x2 << endl;
}