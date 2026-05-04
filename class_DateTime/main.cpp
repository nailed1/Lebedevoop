#include "MoonData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <iomanip>

using namespace std;

// Определение формата файла по заголовку
static bool detectFileType(ifstream& file) {
    string header;
    getline(file, header);
    // Если в заголовке есть "T", значит есть колонка с температурой
    return header.find("T") != string::npos;
}

// Основная функция обработки
MoonResult processMoonData(const DateTime& target) {
    MoonResult result;
    
    // Формируем имя файла (безопасный способ)
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
    
    // Читаем все строки файла
    string line;
    int lineNum = 0;
    bool foundTargetDate = false;
    double prevEl = -999.0;
    double maxEl = -999.0;
    
    while (getline(file, line)) {
        lineNum++;
        
        int ymd, hms;
        double r, el, az, fi, lg;
        double t = 0; // возможно, не используется
        
        // Парсим строку
        stringstream ss(line);
        ss >> ymd >> hms;
        
        if (ss.fail()) {
            cerr << "Warning: Cannot parse line " << lineNum << ": " << line << endl;
            continue;
        }
        
        if (hasTColumn) {
            ss >> t;
        }
        
        ss >> r >> el >> az >> fi >> lg;
        
        if (ss.fail()) {
            cerr << "Warning: Cannot parse data on line " << lineNum << endl;
            continue;
        }
        
        // Извлекаем дату
        int year = ymd / 10000;
        int month = (ymd / 100) % 100;
        int day = ymd % 100;
        
        // Проверяем, та ли это дата
        if (year == target.getYear() && month == target.getMonth() && day == target.getDay()) {
            foundTargetDate = true;
            
            // Извлекаем время
            int hour = hms / 10000;
            int minute = (hms / 100) % 100;
            int second = hms % 100;
            
            DateTime dt(year, month, day, hour, minute, second);
            
            // Проверяем кульминацию
            if (el > maxEl) {
                maxEl = el;
                result.culminationTime = dt;
                result.hasCulmination = true;
            }
            
            // Проверяем восход
            if (prevEl < 0 && el > 0 && prevEl != -999.0) {
                result.riseTime = dt;
                result.hasRise = true;
            }
            
            // Проверяем заход
            if (prevEl >= 0 && el < 0 && prevEl != -999.0) {
                result.setTime = dt;
                result.hasSet = true;
            }
            
            prevEl = el;
        } 
        else if (foundTargetDate) {
            // Мы уже прошли целевую дату - можно выходить
            break;
        }
    }
    
    file.close();
    cout << "Processed " << lineNum << " lines" << endl;
    
    return result;
}

void printResult(const DateTime& target, const MoonResult& result) {
    cout << "\n=== Moon Data for " << target << " ===" << endl;
    
    if (!result.ok) {
        cout << "Error: Cannot open data file" << endl;
        return;
    }
    
    // Вывод времени восхода
    cout << "Rise: ";
    if (result.hasRise) {
        cout << setfill('0') << setw(2) << result.riseTime.getHour() << ":"
             << setfill('0') << setw(2) << result.riseTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.riseTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;
    
    // Вывод времени кульминации
    cout << "Culmination: ";
    if (result.hasCulmination) {
        cout << setfill('0') << setw(2) << result.culminationTime.getHour() << ":"
             << setfill('0') << setw(2) << result.culminationTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.culminationTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;
    
    // Вывод времени захода
    cout << "Set: ";
    if (result.hasSet) {
        cout << setfill('0') << setw(2) << result.setTime.getHour() << ":"
             << setfill('0') << setw(2) << result.setTime.getMinute() << ":"
             << setfill('0') << setw(2) << result.setTime.getSecond();
    } else {
        cout << "---";
    }
    cout << endl;
}