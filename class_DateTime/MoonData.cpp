#include "MoonData.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>

using namespace std;

struct FileAnalysis {
    long long lineStart;
    long long lineLen;
    long long totalLines;
};

bool detectFileType(ifstream& file) {
    char check;
    file >> check;
    return check != '"';
}

FileAnalysis analyzeFile(ifstream& file) {
    file.ignore(1000, '\n');

    long long lineStart = file.tellg(); //получаем 1 байт строки
    file.ignore(1000, '\n');
    long long lineLen = file.tellg() - lineStart; // отнимаем конец от начала

    file.seekg(0, ios::end); // переместили указатель в конец файла
    long long fileSize = file.tellg();

    long long totalLines = (fileSize - lineStart) / lineLen; // кол строк

    return {lineStart, lineLen, totalLines};
}

long long binarySearchDate(ifstream& file, const DateTime& target,
                          long long lineStart, long long lineLen, long long totalLines) {
    long long lo = 0, hi = totalLines - 1;

    while (lo < hi) {
        long long mid = (lo + hi) / 2;

        file.seekg(lineStart + mid * lineLen);
        file.ignore(1000, '\n');

        int ymd;
        file >> ymd;

        DateTime cur(ymd / 10000, (ymd / 100) % 100, ymd % 100);

        if (cur < target) lo = mid + 1;
        else hi = mid;
    }
    
    // откат до 1 нужной даты
    while (lo > 0) {
        file.seekg(lineStart + (lo - 1) * lineLen);
        file.ignore(1000, '\n');
        int ymd;
        file >> ymd;
        int fy = ymd/10000, fm = (ymd/100)%100, fd = ymd%100;
        DateTime cur(fy, fm, fd);
        if (cur < target) break;
        lo--;
    }

    return lo;
}

void processData(ifstream& file, const DateTime& target,
                 long long pos, long long lineStart, long long lineLen,
                 bool typeUsual, MoonResult& result) {

    file.seekg(lineStart + pos * lineLen);
    file.ignore(1000, '\n');

    double prevEl = -999;
    double maxEl = -999;

    int ymd, hms;
    double t, r, el, az, fi, lg;

    while (file >> ymd >> hms) {
        if (typeUsual)
            file >> t >> r >> el >> az >> fi >> lg;
        else
            file >> r >> el >> az >> fi >> lg;

        DateTime cur(ymd / 10000, ymd / 100 % 100, ymd % 100);
        if (cur > target) break;

        DateTime dt(
            ymd / 10000, ymd / 100 % 100, ymd % 100,
            hms / 10000, hms / 100 % 100, hms % 100
        );

        if (el > maxEl) {
            maxEl = el;
            result.culminationTime = dt;
            result.hasCulmination = true;
        }

        if (prevEl < 0 && el > 0 && prevEl != -999) {
            result.riseTime = dt;
            result.hasRise = true;
        }

        if (prevEl >= 0 && el < 0 && prevEl != -999) {
            result.setTime = dt;
            result.hasSet = true;
        }

        prevEl = el;
    }
}

MoonResult processMoonData(const DateTime& target) {
    MoonResult result;

    char filename[40];
    sprintf(filename, "Moon/moon%d.dat", target.getYear());

    ifstream file(filename);
    if (!file.is_open()) {
        result.ok = false;
        return result;
    }

    bool typeUsual = detectFileType(file);

    FileAnalysis analysis = analyzeFile(file);

    long long pos = binarySearchDate(file, target, analysis.lineStart, analysis.lineLen, analysis.totalLines);

    processData(file, target, pos, analysis.lineStart, analysis.lineLen, typeUsual, result);

    return result;
}

DateTime withFormat(DateTime dt, DateTimeFormat format) {
    dt.setFormat(format);
    return dt;
}

void printResult(const DateTime& target, const MoonResult& r) {
    DateTime t = withFormat(target, TIME_ONLY);
    DateTime rise = withFormat(r.riseTime, TIME_ONLY);
    DateTime set = withFormat(r.setTime, TIME_ONLY);
    DateTime culm = withFormat(r.culminationTime, TIME_ONLY);

    cout << "Date: " << t << endl;

    if (r.hasRise) cout << "Rise: " << rise;
    else cout << "Rise: not found"<<endl;
    if (r.hasCulmination) cout << "Culmination: " << culm;
    else cout << "Culmination: not found"<<endl;
    if (r.hasSet) cout << "Set: " << set;
    else cout << "Set: not found"<<endl;
}