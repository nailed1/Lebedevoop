#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
#include <iomanip>

class DateTimeException {};

class DateTime {
private:
    int year, month, day;
    int hour, minute, second;

    static bool isLeapYear(int y);
    static int daysInMonth(int m, int y);
    static bool isValidDate(int y, int m, int d);

public:
    // Конструкторы
    DateTime();
    DateTime(int y, int m, int d, int h = 0, int min = 0, int sec = 0);

    // Геттеры
    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getHour() const { return hour; }
    int getMinute() const { return minute; }
    int getSecond() const { return second; }

    // Сеттеры
    void setYear(int y) { year = y; }
    void setMonth(int m) { month = m; }
    void setDay(int d) { day = d; }

    // Операторы сравнения
    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;

    // Ввод/вывод
    friend std::ostream& operator<<(std::ostream& os, const DateTime& dt);
    friend std::istream& operator>>(std::istream& is, DateTime& dt);

    // Утилиты
    int getDayOfWeek() const;
    static DateTime getEaster(int year);
    DateTime addDays(int days) const;
    int daysBetween(const DateTime& other) const;
};

#endif // DATETIME_H