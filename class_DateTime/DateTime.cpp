#include <iostream>
#include "DateTime.h"
#include <cmath>

using namespace std;

bool DateTime::IsLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTime::DaysInMonth(int month) const {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && IsLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

void DateTime::IsCorrect() {
    while (second < 0) {
        second += 60;
        minute--;
    }
    while (second >= 60) {
        second -= 60;
        minute++;
    }
    
    while (minute < 0) {
        minute += 60;
        hour--;
    }
    while (minute >= 60) {
        minute -= 60;
        hour++;
    }
    
    while (hour < 0) {
        hour += 24;
        day--;
    }
    while (hour >= 24) {
        hour -= 24;
        day++;
    }
    
    while (day < 1) {
        month--;
        if (month < 1) {
            month = 12;
            year--;
        }
        day += DaysInMonth(month);
    }
    while (day > DaysInMonth(month)) {
        day -= DaysInMonth(month);
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
}

long long DateTime::gregorianToJulian(int y, int m, int d) {
    if (m <= 2) {
        y--;
        m += 12;
    }
    return 365LL * y + y / 4 - y / 100 + y / 400 + (153LL * (m - 3) + 2) / 5 + d - 1;
}

DateTime::DateTime() : second(0), minute(0), hour(0), day(1), month(1), year(1970) {}

DateTime::DateTime(int y, int m, int d, int h, int min, int s) 
    : second(s), minute(min), hour(h), day(d), month(m), year(y) {
    IsCorrect();
}

void DateTime::setDate(int y, int m, int d) {
    year = y;
    month = m;
    day = d;
    IsCorrect();
}

void DateTime::setTime(int h, int min, int s) {
    hour = h;
    minute = min;
    second = s;
    IsCorrect();
}

void DateTime::setDateTime(int y, int m, int d, int h, int min, int s) {
    year = y;
    month = m;
    day = d;
    hour = h;
    minute = min;
    second = s;
    IsCorrect();
}

void DateTime::setDateTime(const char* iso_string) {
    int y, m, d, h, min, s;
    sscanf(iso_string, "%d-%d-%dT%d:%d:%d", &y, &m, &d, &h, &min, &s);
    setDateTime(y, m, d, h, min, s);
}

void DateTime::input(istream& in) {
    in >> year >> month >> day >> hour >> minute >> second;
    IsCorrect();
}

void DateTime::print(ostream& out) const {
    out << year << "-" << month << "-" << day << "T" 
        << hour << ":" << minute << ":" << second;
}

void DateTime::printFormat1(ostream& out) const {
    if (day < 10) out << "0";
    out << day << ".";
    if (month < 10) out << "0";
    out << month << "." << year;
}

void DateTime::printFormat2(ostream& out) const {
    const char* months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    out << day << " " << months[month - 1] << " " << year;
}

void DateTime::printFormat3(ostream& out) const {
    int yy = year % 100;
    if (yy < 10) out << "0";
    out << yy << ".";
    if (month < 10) out << "0";
    out << month << ".";
    if (day < 10) out << "0";
    out << day;
}

long long DateTime::differenceInDays(const DateTime& other) const {
    long long j1 = gregorianToJulian(year, month, day);
    long long j2 = gregorianToJulian(other.year, other.month, other.day);
    return j1 - j2;
}

int DateTime::getDayOfWeek() const {
    long long jd = gregorianToJulian(year, month, day);
    return (jd + 1) % 7;
}

int DateTime::calculateEasterDate(int year) const {
    int a = year % 19;
    int b = year / 100;
    int c = year % 100;
    int d = b / 4;
    int e = b % 4;
    int f = (b + 8) / 25;
    int g = (b - f + 1) / 3;
    int h = (19 * a + b - d - g + 15) % 30;
    int i = c / 4;
    int k = c % 4;
    int l = (32 + 2 * e + 2 * i - h - k) % 7;
    int m = (a + 11 * h + 22 * l) / 451;
    int month = (h + l - 7 * m + 114) / 31;
    int day = ((h + l - 7 * m + 114) % 31) + 1;
    
    int dayOfYear = day;
    for (int m = 1; m < month; m++) {
        int daysInM = 31;
        if (m == 4 || m == 6 || m == 9 || m == 11) daysInM = 30;
        else if (m == 2) daysInM = IsLeapYear(year) ? 29 : 28;
        dayOfYear += daysInM;
    }
    return dayOfYear;
}

DateTime DateTime::operator+(int seconds) const {
    DateTime result = *this;
    result.second += seconds;
    result.IsCorrect();
    return result;
}

DateTime DateTime::operator-(int seconds) const {
    return *this + (-seconds);
}

DateTime DateTime::operator-(const DateTime& other) const {
    long long daysDiff = differenceInDays(other);
    long long totalSeconds = daysDiff * 86400LL;
    totalSeconds += (hour - other.hour) * 3600LL;
    totalSeconds += (minute - other.minute) * 60LL;
    totalSeconds += (second - other.second);
    
    bool negative = totalSeconds < 0;
    if (negative) totalSeconds = -totalSeconds;
    
    long long days = totalSeconds / 86400LL;
    int remaining = totalSeconds % 86400LL;
    int hours = remaining / 3600;
    remaining %= 3600;
    int minutes = remaining / 60;
    int seconds = remaining % 60;
    
    if (negative) {
        days = -days;
        hours = -hours;
        minutes = -minutes;
        seconds = -seconds;
    }
    
    return DateTime(0, 0, days, hours, minutes, seconds);
}

DateTime DateTime::operator+(const DateTime& other) const {
    long long totalSeconds = other.hour * 3600LL + other.minute * 60LL + other.second;
    return *this + totalSeconds;
}

bool DateTime::operator==(const DateTime& other) const {
    return year == other.year && month == other.month && day == other.day &&
           hour == other.hour && minute == other.minute && second == other.second;
}

bool DateTime::operator!=(const DateTime& other) const {
    return !(*this == other);
}

bool DateTime::operator<(const DateTime& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    if (day != other.day) return day < other.day;
    if (hour != other.hour) return hour < other.hour;
    if (minute != other.minute) return minute < other.minute;
    return second < other.second;
}

bool DateTime::operator>(const DateTime& other) const {
    return other < *this;
}

bool DateTime::operator<=(const DateTime& other) const {
    return !(*this > other);
}

bool DateTime::operator>=(const DateTime& other) const {
    return !(*this < other);
}
