#include "MoonData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <iomanip>

using namespace std;

static int partition(std::vector<DateTime>& dates, int low, int high) {
    DateTime pivot = dates[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (dates[j] <= pivot) {
            i++;
            DateTime tmp = dates[i];
            dates[i] = dates[j];
            dates[j] = tmp;
        }
    }
    DateTime tmp = dates[i + 1];
    dates[i + 1] = dates[high];
    dates[high] = tmp;
    return i + 1;
}

static void quickSort(std::vector<DateTime>& dates, int low, int high) {
    if (low < high) {
        int pi = partition(dates, low, high);
        quickSort(dates, low, pi - 1);
        quickSort(dates, pi + 1, high);
    }
}

void sortDates(std::vector<DateTime>& dates) {
    if (dates.size() > 1)
        quickSort(dates, 0, (int)dates.size() - 1);
}

static bool detectFileType(ifstream& file) {
    string header;
    getline(file, header);
    return header.find("T") != string::npos;
}

// Парсит первое поле (ymd) из строки, возвращает -1 при ошибке
static int parseYmd(const string& line) {
    istringstream ss(line);
    int ymd;
    ss >> ymd;
    return ss.fail() ? -1 : ymd;
}

// Бинарный поиск по файлу: возвращает смещение первой строки, где ymd >= targetYmd.
// lo — начало данных (после заголовка), hi — размер файла.
// Инвариант: lo всегда указывает на начало строки.
static long findStartOfDate(ifstream& file, int targetYmd, long lo, long hi) {
    while (lo < hi) {
        long mid = lo + (hi - lo) / 2;
        file.seekg(mid);

        // Если не на границе строки — досчитываем текущую строку до конца
        if (mid > lo) {
            string dummy;
            getline(file, dummy);
        }

        long lineStart = (long)file.tellg();

        // Если вышли за верхнюю границу — сужаем правую часть
        if (lineStart < 0 || lineStart >= hi) {
            hi = mid;
            continue;
        }

        string line;
        if (!getline(file, line)) {
            hi = mid;
            break;
        }

        int ymd = parseYmd(line);

        if (ymd >= 0 && ymd < targetYmd) {
            lo = (long)file.tellg(); // строка меньше цели — ищем правее
        } else {
            hi = lineStart;          // строка >= цели — ищем левее (или это и есть граница)
        }
    }
    return lo;
}

MoonResult processMoonData(const DateTime& target) {
    MoonResult result;

    char filename[50];
    snprintf(filename, sizeof(filename), "Moon/moon%d.dat", target.getYear());

    ifstream file(filename);
    if (!file.is_open()) {
        result.ok = false;
        return result;
    }

    cout << "Opened file: " << filename << endl;

    bool hasTColumn = detectFileType(file);
    cout << (hasTColumn ? "Full format" : "Short format") << endl;

    long dataStart = (long)file.tellg();

    file.seekg(0, ios::end);
    long fileEnd = (long)file.tellg();

    int targetYmd = target.getYear() * 10000 + target.getMonth() * 100 + target.getDay();

    // O(log N) — находим начало нужной даты
    long startPos = findStartOfDate(file, targetYmd, dataStart, fileEnd);
    file.seekg(startPos);

    string line;
    int lineNum = 0;
    double prevEl = -999.0;
    double maxEl  = -999.0;

    // O(k) — читаем только строки нужного дня
    while (getline(file, line)) {
        lineNum++;

        int ymd, hms;
        double r, el, az, fi, lg;
        double t = 0;

        istringstream ss(line);
        ss >> ymd >> hms;
        if (ss.fail()) continue;

        if (ymd != targetYmd) break;

        if (hasTColumn) ss >> t;
        ss >> r >> el >> az >> fi >> lg;
        if (ss.fail()) continue;

        int year   = ymd / 10000;
        int month  = (ymd / 100) % 100;
        int day    = ymd % 100;
        int hour   = hms / 10000;
        int minute = (hms / 100) % 100;
        int second = hms % 100;

        DateTime dt(year, month, day, hour, minute, second);

        if (el > maxEl) {
            maxEl = el;
            result.culminationTime = dt;
            result.hasCulmination = true;
        }

        if (prevEl < 0 && el >= 0 && prevEl != -999.0) {
            result.riseTime = dt;
            result.hasRise = true;
        }

        if (prevEl >= 0 && el < 0 && prevEl != -999.0) {
            result.setTime = dt;
            result.hasSet = true;
        }

        prevEl = el;
    }

    file.close();
    cout << "Processed " << lineNum << " lines for target date" << endl;

    return result;
}

void printResult(const DateTime& target, const MoonResult& result) {
    cout << "\n=== Moon Data for " << target << " ===" << endl;

    if (!result.ok) {
        cout << "Error: Cannot open data file" << endl;
        return;
    }

    cout << "Rise: ";
    if (result.hasRise) {
        cout << setfill('0') << setw(2) << result.riseTime.getHour()   << ":"
             << setfill('0') << setw(2) << result.riseTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.riseTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;

    cout << "Culmination: ";
    if (result.hasCulmination) {
        cout << setfill('0') << setw(2) << result.culminationTime.getHour()   << ":"
             << setfill('0') << setw(2) << result.culminationTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.culminationTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;

    cout << "Set: ";
    if (result.hasSet) {
        cout << setfill('0') << setw(2) << result.setTime.getHour()   << ":"
             << setfill('0') << setw(2) << result.setTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.setTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;
}
