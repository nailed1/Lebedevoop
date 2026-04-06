#include <iostream>

using namespace std;

class DateTime{
    private:
        int second;
        int minute;
        int hour;
        int day;
        int month;
        int year;

        bool IsLeapYear(int year) const;
        int DaysInMonth(int month) const;
        void IsCorrect();
        static long long gregorianToJulian(int y, int m, int d);

    public:
        DateTime();
        DateTime(int y, int m, int d, int h = 0, int min = 0, int s = 0);

        void setDate(int y, int m, int d);
        void setTime(int h, int min, int s);
        void setDateTime(int y, int m, int d, int h, int min, int s);
        void setDateTime(const char* iso_string); // Формат: YYYY-MM-DDThh:mm:ss

        int getYear() const { return year; }
        int getMonth() const { return month; }
        int getDay() const { return day; }
        int getHour() const { return hour; }
        int getMinute() const { return minute; }
        int getSecond() const { return second; }

        // Основные методы задачи
        void input(std::istream& in = std::cin);
        void print(std::ostream& out = std::cout) const;
        void printFormat1(std::ostream& out = std::cout) const; // DD.MM.YYYY
        void printFormat2(std::ostream& out = std::cout) const; // D MonthName YYYY
        void printFormat3(std::ostream& out = std::cout) const; // YY.MM.DD
        long long differenceInDays(const DateTime& other) const;
        int getDayOfWeek() const;
        int calculateEasterDate(int year) const;

        
        DateTime operator+(int seconds) const;
        DateTime operator-(int seconds) const;
        DateTime operator-(const DateTime& other) const;
        DateTime operator+(const DateTime& other) const;
        bool operator==(const DateTime& other) const;
        bool operator!=(const DateTime& other) const;
        bool operator<(const DateTime& other) const;
        bool operator>(const DateTime& other) const;
        bool operator<=(const DateTime& other) const;
        bool operator>=(const DateTime& other) const;
};