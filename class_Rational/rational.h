class Rational 
{
private:
    void simplify();


public:
    int num;
    int den;

    //функции
    Rational();
    Rational(int n);
    Rational(int n, int d);
    Rational(double val);
    void quadratic_function(const Rational& a, const Rational& b, const Rational& c);

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
    Rational& operator+=(int x);
    Rational  operator+(int x) const;

    Rational& operator-=(int x);
    Rational  operator-(int x) const;

    Rational& operator*=(int x);
    Rational  operator*(int x) const;

    Rational& operator/=(int x);
    Rational  operator/(int x) const;

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

Rational operator + (int x, const Rational& r);
Rational operator - (int x, const Rational& r);
Rational operator * (int x, const Rational& r);
Rational operator / (int x, const Rational& r);