class Rational 
{
private:
    void simplify();
    // Вспомогательные функции для проверки переполнения
    static bool will_add_overflow(long long a, long long b);
    static bool will_subtract_overflow(long long a, long long b);
    static bool will_multiply_overflow(long long a, long long b);
    static bool will_divide_overflow(long long a, long long b);
    
public:
    long long num;
    long long den;

    //функции
    Rational();
    Rational(int n);
    Rational(long long n);
    Rational(long long n, long long d);
    Rational(double val);


    //мат. операции
    Rational& operator +=(const Rational& r);
    Rational operator + (const Rational& r) const;
    Rational& operator -=(const Rational& r);
    Rational operator - (const Rational& r) const;
    Rational& operator *=(const Rational& r);
    Rational operator * (const Rational& r) const;
    Rational& operator /=(const Rational& r);
    Rational operator / (const Rational& r) const;

    //мат операции rational с int
    Rational& operator+=(long long x);
    Rational  operator+(long long x) const;

    Rational& operator-=(long long x);
    Rational  operator-(long long x) const;

    Rational& operator*=(long long x);
    Rational  operator*(long long x) const;

    Rational& operator/=(long long x);
    Rational  operator/(long long x) const;

    //перевод rational в другие типы
    operator int () const;
    operator double() const;

    //логические операции с rational
    bool operator==(const Rational& r) const;
    bool operator!=(const Rational& r) const;
    bool operator<(const Rational& r) const;
    bool operator<=(const Rational& r) const;
    bool operator>(const Rational& r) const;
    bool operator>=(const Rational& r) const;
};

Rational operator + (long long x, const Rational& r);
Rational operator - (long long x, const Rational& r);
Rational operator * (long long x, const Rational& r);
Rational operator / (long long x, const Rational& r);