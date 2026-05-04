#include "rational.h"
#include <iostream>
#include <cmath>

using namespace std;

Rational my_abs(Rational r) {
    if (r < Rational(0)) {
        try {
            return Rational(-r.num, r.den);
        } catch (const overflow_error&) {
            return r;
        }
    }
    return r;
}

static long long gcd(long long a, long long b) {
    while (b) { 
        long long t = b; 
        b = a % b; 
        a = t; 
    }
    return a;
}

// Проверка на переполнение при умножении
bool will_multiply_overflow(long long a, long long b) {
    if (a == 0 || b == 0) return false;
    if (a > 0) {
        if (b > 0) {
            if (a > numeric_limits<long long>::max() / b) return true;
        } else {
            if (b < numeric_limits<long long>::min() / a) return true;
        }
    } else {
        if (b > 0) {
            if (a < numeric_limits<long long>::min() / b) return true;
        } else {
            if (a < numeric_limits<long long>::max() / b) return true;
        }
    }
    return false;
}
// Целочисленный корень через long long
long long integer_sqrt_heron(long long N) {
    if (N < 0) throw invalid_argument("Negative radicand");
    if (N == 0 || N == 1) return (long long)N;
    
    long long x = N;
    long long y = (x + 1) / 2;
    
    while (y < x) {
        x = y;
        y = (x + N / x) / 2;
    }
    
    // Проверяем, какой корень ближе: x или x+1
    long long sq1 = x * x;
    long long sq2 = (x + 1) * (x + 1);
    
    if ((N - sq1) > (sq2 - N)) {
        return (long long)(x + 1);
    }
    
    return (long long)x;
}

Rational sqrt_rational(const Rational& D) {
    if (D < Rational(0))
        throw invalid_argument("Negative radicand");
    if (D == Rational(0))
        return Rational(0);
    
    long long a = D.num;
    long long b = D.den;
    
    // Упрощаем дробь
    long long g = gcd(abs(a), abs(b));
    a /= g;
    b /= g;
    
    // Начальное приближение через integer_sqrt
    long long N = a * b;
    long long root = integer_sqrt_heron(N);
    Rational x(root, b);
    
    // Уточнение алгоритмом Герона
    // x_{n+1} = (x_n + D/x_n) / 2
    for (int i = 0; i < 100; i++) {
        try {
            Rational x_next = (x + D / x) / Rational(2);
            
            // Проверяем сходимость
            if (x_next == x) break;
            
            Rational diff = x_next - x;
            if (my_abs(diff) < Rational(1, 1000000)) {
                x = x_next;
                break;
            }
            
            x = x_next;
        } catch (const overflow_error&) {
            // Переполнение на итерации - возвращаем текущее приближение
            break;
        }
    }
    
    return x;
}

struct Roots {
    bool real;
    Rational x1, x2;
};

Roots solve_quadratic(const Rational& a, const Rational& b, const Rational& c) {
    if (a == Rational(0)) throw invalid_argument("Not a quadratic equation");
    
    Rational D = b * b - Rational(4) * a * c;
    cout << "D = " << D.num << "/" << D.den << " = " << (double)D << endl;
    
    if (D < Rational(0)) {
        return {false, Rational(), Rational()};
    }
    
    Rational sqrtD = sqrt_rational(D);
    cout << "sqrtD = " << sqrtD.num << "/" << sqrtD.den << " = " << (double)sqrtD << endl;
    
    Rational two_a = Rational(2) * a;
    Rational minus_b = b * Rational(-1);
    Rational x1 = (minus_b - sqrtD) / two_a;
    Rational x2 = (minus_b + sqrtD) / two_a;
    
    return {true, x1, x2};
}

int main(){
    long long a_num, a_den, b_num, b_den, c_num, c_den;
    
    cout << "Enter a (numerator denominator): ";
    cin >> a_num >> a_den;
    cout << "Enter b (numerator denominator): ";
    cin >> b_num >> b_den;
    cout << "Enter c (numerator denominator): ";
    cin >> c_num >> c_den;
    
    Rational a(a_num, a_den);
    Rational b(b_num, b_den);
    Rational c(c_num, c_den);
    
    Roots res = solve_quadratic(a, b, c);
    
    if (res.real) {
        cout << "x1 = " << res.x1.num << "/" << res.x1.den << " = " << (double)res.x1 << endl;
        cout << "x2 = " << res.x2.num << "/" << res.x2.den << " = " << (double)res.x2 << endl;
    } else {
        cout << "No real roots" << endl;
    }
    
    return 0;
}
// Enter a (numerator denominator): 97 101
// Enter b (numerator denominator): 103 107
// Enter c (numerator denominator): -103 113
// D = 578628353/130667437 = 4.42825
// sqrtD = 274968878/130667437 = 2.10434
// x1 = -400751551/250984978 = -1.59672
// x2 = 149186205/250984978 = 0.594403