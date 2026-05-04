#include "DateTime.h"
#include <stdexcept>

// Вспомогательные статические методы
bool DateTime::isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int DateTime::daysInMonth(int m, int y) {
    switch (m) {
        case 4: case 6: case 9: case 11: return 30;
        case 2: return isLeapYear(y) ? 29 : 28;
        default: return 31;
    }
}

bool DateTime::isValidDate(int y, int m, int d) {
    return y >= 1582 && m >= 1 && m <= 12 && 
           d >= 1 && d <= daysInMonth(m, y);
}

// Конструкторы
DateTime::DateTime() : year(2000), month(1), day(1), 
                       hour(0), minute(0), second(0) {}

DateTime::DateTime(int y, int m, int d, int h, int min, int sec) {
    if (!isValidDate(y, m, d) || h < 0 || h > 23 || 
        min < 0 || min > 59 || sec < 0 || sec > 59)
        throw DateTimeException();
    
    year = y; month = m; day = d;
    hour = h; minute = min; second = sec;
}

// Операторы сравнения
bool DateTime::operator==(const DateTime& other) const {
    return year == other.year && month == other.month && 
           day == other.day && hour == other.hour && 
           minute == other.minute && second == other.second;
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
    return !(other < *this);
}

bool DateTime::operator>=(const DateTime& other) const {
    return !(*this < other);
}

// Ввод/вывод
std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
    if (dt.hour == 0 && dt.minute == 0 && dt.second == 0) {
        // Вывод только даты
        os << dt.year << "-" 
           << std::setw(2) << std::setfill('0') << dt.month << "-"
           << std::setw(2) << std::setfill('0') << dt.day;
    } else if (dt.year == 2000 && dt.month == 1 && dt.day == 1) {
        // Вывод только времени (если дата по умолчанию)
        os << std::setw(2) << std::setfill('0') << dt.hour << ":"
           << std::setw(2) << std::setfill('0') << dt.minute << ":"
           << std::setw(2) << std::setfill('0') << dt.second;
    } else {
        // Полный вывод
        os << dt.year << "-" 
           << std::setw(2) << std::setfill('0') << dt.month << "-"
           << std::setw(2) << std::setfill('0') << dt.day << "T"
           << std::setw(2) << std::setfill('0') << dt.hour << ":"
           << std::setw(2) << std::setfill('0') << dt.minute << ":"
           << std::setw(2) << std::setfill('0') << dt.second;
    }
    return os;
}

std::istream& operator>>(std::istream& is, DateTime& dt) {
    int y = 2000, m = 1, d = 1, h = 0, min = 0, sec = 0;
    char sep1, sep2;
    
    is >> y >> sep1 >> m >> sep2 >> d;
    
    if (sep1 != '-' || sep2 != '-') {
        throw DateTimeException();
    }
    
    // Проверяем, есть ли время
    if (is.peek() == 'T' || is.peek() == ' ') {
        char t;
        is >> t >> h >> sep1 >> min >> sep2 >> sec;
    }
    
    dt = DateTime(y, m, d, h, min, sec);
    return is;
}

// Утилиты
int DateTime::getDayOfWeek() const {
    // Формула Зеллера
    int m = month;
    int y = year;
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    
    int K = y % 100;
    int J = y / 100;
    
    int h = (day + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;
    
    // Преобразуем: суббота=0, воскресенье=1 -> понедельник=1, воскресенье=7
    int dow = ((h + 5) % 7) + 1;
    return dow;
}

DateTime DateTime::getEaster(int year) {
    // Алгоритм Гаусса
    int a = year % 19;
    int b = year % 4;
    int c = year % 7;
    int d = (19 * a + 24) % 30;
    int e = (2 * b + 4 * c + 6 * d + 5) % 7;
    
    int day = 22 + d + e;
    int month = 3;
    
    if (day > 31) {
        day -= 31;
        month = 4;
    }
    
    // Корректировки для особых случаев
    if (day == 26 && month == 4) day = 19;
    if (day == 25 && month == 4 && d == 28 && a > 10) day = 18;
    
    return DateTime(year, month, day);
}

DateTime DateTime::addDays(int days) const {
    int d = day + days;
    int m = month;
    int y = year;
    
    while (d > daysInMonth(m, y)) {
        d -= daysInMonth(m, y);
        m++;
        if (m > 12) {
            m = 1;
            y++;
        }
    }
    
    while (d < 1) {
        m--;
        if (m < 1) {
            m = 12;
            y--;
        }
        d += daysInMonth(m, y);
    }
    
    return DateTime(y, m, d, hour, minute, second);
}

int DateTime::daysBetween(const DateTime& other) const {
    DateTime dt1 = *this;
    DateTime dt2 = other;
    
    // Считаем дни от некоторой точки
    auto daysFromEpoch = [](const DateTime& dt) -> int {
        int days = dt.getDay();
        for (int m = 1; m < dt.getMonth(); m++) {
            days += daysInMonth(m, dt.getYear());
        }
        for (int y = 1582; y < dt.getYear(); y++) {
            days += isLeapYear(y) ? 366 : 365;
        }
        return days;
    };
    
    return daysFromEpoch(dt2) - daysFromEpoch(dt1);
}