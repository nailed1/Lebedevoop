#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
using namespace std;

class DateTimeException {};

enum DateTimeFormat { FULL, DATE_ONLY, TIME_ONLY };

class DateTime{
    double data;
    DateTimeFormat fmt;

    static bool isLeap(int y);
    static int daysInMonth(int m, int y);
    static double dateToDouble(int y, int m, int d, int h, int min, int sec);
public:
    DateTime() : data(0), fmt(FULL) {};
    DateTime(int y,int m,int d,int h=0, int min=0,int sec=0, DateTimeFormat f = FULL);
    void setFormat(DateTimeFormat f) { fmt = f; }

    friend ostream& operator<<(ostream& os, const DateTime& dt);
    friend istream& operator>>(istream& is, DateTime& dt);

    double operator-(const DateTime& other) const;
    DateTime operator+(int days) const;

    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;

    int getDayOfWeek() const;
    int getYear() const;
    static DateTime easter(int year);
};

#endif