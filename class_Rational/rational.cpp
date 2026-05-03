#include "rational.h"
#include <iostream>
#include <limits>
#include <cmath>

using namespace std;

static long long gcd(long long a, long long b) {
    while (b) { long long t = b; b = a % b; a = t; }
    return a;
}

// Проверки переполнения с использованием __int128
bool Rational::will_add_overflow(long long a, long long b) {
    if (a > 0 && b > 0 && a > numeric_limits<long long>::max() - b)
        return true;
    if (a < 0 && b < 0 && a < numeric_limits<long long>::min() - b)
        return true;
    return false;
}

bool Rational::will_subtract_overflow(long long a, long long b) {
    if (b == numeric_limits<long long>::min()) {
        return a >= 0;  // a - LLONG_MIN переполнится для любого a >= 0
    }
    if (a < 0 && b > 0 && a < numeric_limits<long long>::min() + b)
        return true;
    if (a > 0 && b < 0 && a > numeric_limits<long long>::max() + b)
        return true;
    return false;
}

bool Rational::will_multiply_overflow(long long a, long long b) {
    if (a == 0 || b == 0) return false;
    
    // Особый случай: LLONG_MIN * -1
    if (a == numeric_limits<long long>::min() && b == -1) return true;
    if (b == numeric_limits<long long>::min() && a == -1) return true;
    
    // Используем беззнаковое умножение для проверки
    unsigned long long ua = a < 0 ? -(unsigned long long)a : a;
    unsigned long long ub = b < 0 ? -(unsigned long long)b : b;
    
    if (ua > numeric_limits<unsigned long long>::max() / ub)
        return true;
    
    return false;
}

bool Rational::will_divide_overflow(long long a, long long b) {
    if (b == 0) throw invalid_argument("Division by zero");
    // Особый случай: LONG_LONG_MIN / -1
    if (a == numeric_limits<long long>::min() && b == -1) return true;
    return false;
}

//Определение рациональной дроби

Rational::Rational() {
    num = 0;
    den = 1;
}

Rational::Rational(int n) {
    num = n;
    den = 1;
}

Rational::Rational(long long n) {
    num = n;
    den = 1;
}

Rational::Rational(long long n, long long d) {
    num = n;
    den = d;
    simplify();
}
//опреаторы сложение, вычитания, умножения, деления

Rational& Rational::operator+=(const Rational& r) {
    // Проверяем умножения для общего знаменателя
    if (will_multiply_overflow(num, r.den)) 
        throw overflow_error("Addition: num * r.den overflow");
    if (will_multiply_overflow(den, r.num)) 
        throw overflow_error("Addition: den * r.num overflow");
    if (will_multiply_overflow(den, r.den)) 
        throw overflow_error("Addition: den * r.den overflow");
    
    long long new_num = (long long)num * r.den + (long long)den * r.num;
    long long new_den = (long long)den * r.den;
    
    // Проверяем сложение числителей
    if (will_add_overflow((long long)num * r.den, (long long)den * r.num))
        throw overflow_error("Addition: numerator addition overflow");
    
    num = new_num;
    den = new_den;
    simplify();
    return *this;
}


Rational Rational::operator+(const Rational& r) const {
    Rational R(*this);
    return R += r;
}

Rational& Rational::operator-=(const Rational& r) {
    // Проверяем умножения для общего знаменателя
    if (will_multiply_overflow(num, r.den)) 
        throw overflow_error("Subtraction: num * r.den overflow");
    if (will_multiply_overflow(den, r.num)) 
        throw overflow_error("Subtraction: den * r.num overflow");
    if (will_multiply_overflow(den, r.den)) 
        throw overflow_error("Subtraction: den * r.den overflow");
    
    long long new_num = (long long)num * r.den - (long long)den * r.num;
    long long new_den = (long long)den * r.den;
    
    // Проверяем вычитание числителей
    if (will_subtract_overflow((long long)num * r.den, (long long)den * r.num))
        throw overflow_error("Subtraction: numerator subtraction overflow");
    
    num = new_num;
    den = new_den;
    simplify();
    return *this;
}

Rational Rational::operator-(const Rational& r) const {
    Rational R(*this);
    return R -= r;
}

Rational& Rational::operator*=(const Rational& r) {
    // Сначала сокращаем крест-накрест
    long long g1 = gcd(abs(num), abs(r.den));
    long long g2 = gcd(abs(r.num), abs(den));
    
    long long new_num1 = num / g1;
    long long new_num2 = r.num / g2;
    long long new_den1 = den / g2;
    long long new_den2 = r.den / g1;
    
    // Проверяем переполнение после сокращения
    if (will_multiply_overflow(new_num1, new_num2))
        throw overflow_error("Multiplication: numerator overflow");
    if (will_multiply_overflow(new_den1, new_den2))
        throw overflow_error("Multiplication: denominator overflow");
    
    num = new_num1 * new_num2;
    den = new_den1 * new_den2;
    
    if (den < 0) { num = -num; den = -den; }
    return *this;
}

Rational Rational::operator*(const Rational& r) const {
    Rational R(*this);
    return R *= r;
}

Rational& Rational::operator/=(const Rational& r) {
    if (r.num == 0) throw invalid_argument("Division by zero");
    
    // Сокращаем крест-накрест
    long long g1 = gcd(abs(num), abs(r.num));
    long long g2 = gcd(abs(den), abs(r.den));
    
    long long new_num1 = num / g1;
    long long new_den2 = r.den / g2;
    long long new_den1 = den / g2;
    long long new_num2 = r.num / g1;
    
    // Проверяем переполнение после сокращения
    if (will_multiply_overflow(new_num1, new_den2))
        throw overflow_error("Division: numerator overflow");
    if (will_multiply_overflow(new_den1, new_num2))
        throw overflow_error("Division: denominator overflow");
    
    // Проверка деления LONG_LONG_MIN на -1
    if (will_divide_overflow(new_num1 * new_den2, 1)) // формальная проверка
        throw overflow_error("Division: result overflow");
    
    num = new_num1 * new_den2;
    den = new_den1 * new_num2;
    
    if (den < 0) { num = -num; den = -den; }
    return *this;
}

Rational Rational::operator/(const Rational& r) const {
    Rational R(*this);
    return R /= r;
}

//мат операции rational с int
Rational& Rational::operator+=(long long x) {
    return *this += Rational(x);
}

Rational Rational::operator+(long long x) const {
    Rational tmp(*this);
    tmp += x;
    return tmp;
}

Rational& Rational::operator-=(long long x) {
    return *this -= Rational(x);
}

Rational Rational::operator-(long long x) const {
    Rational tmp(*this);
    tmp -= x;
    return tmp;
}

Rational& Rational::operator*=(long long x) {
    return *this *= Rational(x);
}

Rational Rational::operator*(long long x) const {
    Rational tmp(*this);
    tmp *= x;
    return tmp;
}

Rational& Rational::operator/=(long long x) {
    return *this /= Rational(x);
}

Rational Rational::operator/(long long x) const {
    Rational tmp(*this);
    tmp /= x;
    return tmp;
}


//функция упрощения рациональной дроби с помощью алгоритма Евклида

void Rational::simplify() {
    long long a = num < 0 ? -num : num;
    long long b = den < 0 ? -den : den;
    while (b!=0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    long long commonDivisor = a; // НОД

    num /= commonDivisor;
    den /= commonDivisor;

    if (den < 0) {
        num = -num;
        den = -den;
    }
}


//операторы перевода рациональной дроби в int (с потерей) и double

Rational::operator int() const {
    return num / den;
}

Rational::operator double() const {
    return ((double)num)/den;
}

//альтернативный вариант с использованием мантиссы и экспоненты

Rational::Rational(double val) {
    int exp;
    long long m = llround(frexp(val, &exp) * (1LL << 30)); // 30 бит — влезает в int
    num = (int)m;
    den = (1 << (30 - exp));
    simplify();
}

//лог. операции

bool Rational::operator==(const Rational& r) const {
    return num == r.num && den == r.den;
}

bool Rational::operator!=(const Rational& r) const {
    return !(*this == r);
}

bool Rational::operator<(const Rational& r) const {
    return num * r.den < r.num * den;
}

bool Rational::operator<=(const Rational& r) const {
    return *this < r || *this == r;
}

bool Rational::operator>(const Rational& r) const {
    return !(*this <= r);
}

bool Rational::operator>=(const Rational& r) const {
    return !(*this < r);
}

Rational operator+(long long x, const Rational& r) {
    return Rational(x) + r;
}

Rational operator-(long long x, const Rational& r) {
    return Rational(x) - r;
}

Rational operator*(long long x, const Rational& r) {
    return Rational(x) * r;
}

Rational operator/(long long x, const Rational& r) {
    return Rational(x) / r;
}