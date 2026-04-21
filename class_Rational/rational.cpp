#include "rational.h"
#include <iostream>
#include <limits.h>
#include <cmath>

using namespace std;

static long long gcd(long long a, long long b) {
    while (b) { long long t = b; b = a % b; a = t; }
    return a;
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
    num = (long long)num * r.den + (long long)den * r.num;
    den = (long long)den * r.den;
    simplify();
    return *this;
}


Rational Rational::operator+(const Rational& r) const {
    Rational R(*this);
    return R += r;
}

Rational& Rational::operator-=(const Rational& r) {
    num = (long long)num * r.den - (long long)den * r.num;
    den = (long long)den * r.den;
    simplify();
    return *this;
}

Rational Rational::operator-(const Rational& r) const {
    Rational R(*this);
    return R -= r;
}

Rational& Rational::operator*=(const Rational& r) {
    // Сокращаем крест-накрест до умножения
    long long g1 = gcd(abs(num), abs(r.den));
    long long g2 = gcd(abs(r.num), abs(den));
    
    num = (num / g1) * (r.num / g2);
    den = (den / g2) * (r.den / g1);
    
    if (den < 0) { num = -num; den = -den; }
    return *this;
}

Rational Rational::operator*(const Rational& r) const {
    Rational R(*this);
    return R *= r;
}

Rational& Rational::operator/=(const Rational& r) {
    long long g1 = gcd(abs(num), abs(r.num));
    long long g2 = gcd(abs(den), abs(r.den));
    
    num = (num / g1) * (r.den / g2);
    den = (den / g2) * (r.num / g1);
    
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