#include "DateTime.h"
#include <iostream>
#include <math.h>

using namespace std;

bool DateTime::isLeap(int y){
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int DateTime::daysInMonth(int m, int y){
    if (m==4 || m==6 || m==9 || m==11)
        return 30;
    if (m==2)
        return isLeap(y) ? 29 : 28;
    return 31;
}

double DateTime::dateToDouble(int y, int m, int d, int h, int min, int sec){
    if (y<1582 || m<1 || m>12 || h<0 || h>23 || min<0 || min>59 || sec<0 || sec>59)
        throw DateTimeException();
    if (d<1 || d>daysInMonth(m,y))
        throw DateTimeException();

    // Алгоритм Жана Меёса
    if (m <= 2) {
        y -= 1;
        m += 12;
    }

    int A = y / 100;
    int B = 2 - A + (A / 4);

    double jd_day = (long long)(365.25 * (y + 4716)) + 
                    (long long)(30.6001 * (m + 1)) + 
                    d + B - 1524.5;

    double jd_time = (h * 3600.0 + min * 60.0 + sec) / 86400.0;

    return jd_day + jd_time;
}

DateTime::DateTime(int y,int m,int d,int h, int min,int sec, DateTimeFormat f){
    data=dateToDouble(y, m, d, h, min, sec);
    fmt=f;
}

istream& operator>>(istream& is, DateTime& dt) {
    int v1, v2, v3;
    char s1, s2;
    int y = 0, m = 1, d = 1, h = 0, min = 0, sec = 0;

    if (!(is >> v1 >> s1 >> v2 >> s2 >> v3))
        throw DateTimeException();

    if (s1 == ':') {
        h = v1; min = v2; sec = v3;
        dt.setFormat(TIME_ONLY);
    } else if (s1 == '-') {
        y = v1; m = v2; d = v3;
        dt.setFormat(DATE_ONLY);
        if (is.peek() == 'T') {
            char t, s4, s5;
            is >> t >> h >> s4 >> min >> s5 >> sec;
            dt.setFormat(FULL);
        }
    } else {
        throw DateTimeException();
    }

    dt.data = DateTime::dateToDouble(y, m, d, h, min, sec);
    return is;
}


ostream& operator<<(ostream& os, const DateTime& dt) {
    double z = floor(dt.data + 0.5);
    double f = (dt.data + 0.5) - z;
    double a;

    if (z < 2299161) {
        a = z;
    } else {
        double alpha = floor((z - 1867216.25) / 36524.25);
        a = z + 1 + alpha - floor(alpha / 4);
    }

    double b = a + 1524;
    double c = floor((b - 122.1) / 365.25);
    double d_ptr = floor(365.25 * c);
    double e = floor((b - d_ptr) / 30.6001);

    int day   = (int)(b - d_ptr - floor(30.6001 * e));
    int month = (int)((e < 14) ? (e - 1) : (e - 13));
    int year  = (int)((month > 2) ? (c - 4716) : (c - 4715));
    int h     = (int)(f * 24) % 24;
    int min   = (int)(f * 1440) % 60;
    int sec   = (int)(f * 86400) % 60;

    if (dt.fmt == DATE_ONLY) {
        os << year << "-"
           << (month < 10 ? "0" : "") << month << "-"
           << (day   < 10 ? "0" : "") << day << "\n";
    } 
    else if (dt.fmt == TIME_ONLY) {
        os << (h   < 10 ? "0" : "") << h   << ":"
           << (min < 10 ? "0" : "") << min << ":"
           << (sec < 10 ? "0" : "") << sec << "\n";
    } 
    else {
        os << year << "-"
           << (month < 10 ? "0" : "") << month << "-"
           << (day   < 10 ? "0" : "") << day   << "T"
           << (h     < 10 ? "0" : "") << h     << ":"
           << (min   < 10 ? "0" : "") << min   << ":"
           << (sec   < 10 ? "0" : "") << sec   << "\n";
    }
    return os;
}


double DateTime::operator-(const DateTime& other) const {return data-other.data;}
DateTime DateTime::operator+(int days) const {
    DateTime buf=*this;
    buf.data += days;
    return buf;
}

bool DateTime::operator==(const DateTime& other) const {return data == other.data;}
bool DateTime::operator!=(const DateTime& other) const {return data != other.data;}
bool DateTime::operator>(const DateTime& other) const {return data > other.data;}
bool DateTime::operator>=(const DateTime& other) const {return data >= other.data;}
bool DateTime::operator<(const DateTime& other) const {return data < other.data;}
bool DateTime::operator<=(const DateTime& other) const {return data <= other.data;}


int DateTime::getDayOfWeek() const {
    int dow = (int)(floor(data + 1.5)) % 7;
    if (dow < 0) dow += 7;
    return dow == 0 ? 7 : dow;
}

int DateTime::getYear() const {
    double z = floor(data + 0.5);
    double f = (data + 0.5) - z;
    double a;

    if (z < 2299161) {
        a = z;
    } else {
        double alpha = floor((z - 1867216.25) / 36524.25);
        a = z + 1 + alpha - floor(alpha / 4);
    }

    double b = a + 1524;
    double c = floor((b - 122.1) / 365.25);
    double d_ptr = floor(365.25 * c);
    double e = floor((b - d_ptr) / 30.6001);

    int month = (int)((e < 14) ? (e - 1) : (e - 13));
    int year  = (int)((month > 2) ? (c - 4716) : (c - 4715));
    return year;
}

DateTime DateTime::easter(int year) {
    // Алгоритм Гаусса
    int a = year % 19;
    int b = year % 4;
    int c = year % 7;
    int d = (19 * a + 24) % 30;
    int e = (2 * b + 4 * c + 6 * d + 5) % 7;
    int day = 22 + d + e;
    int month = 3;

    if (day == 57) { day = 19; month = 4; }
    if (d == 28 && e == 6 && a > 10) { day = 18; month = 4; }

    if (day > 31) { day -= 31; month = 4; }

    return DateTime(year, month, day, 0, 0, 0);
}