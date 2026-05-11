#include "DateTime.h"
#include "MoonData.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Простая функция для тестирования DateTime
void testDateTime() {
    cout << "=== DateTime Tests ===" << endl;
    
    try {
        // Создание дат
        DateTime dt1(2022, 3, 15);
        DateTime dt2(2022, 3, 15, 14, 30, 0);
        DateTime dt3(2022, 6, 21);
        
        cout << "Date only: " << dt1 << endl;
        cout << "Date with time: " << dt2 << endl;
        
        // Сравнения
        cout << "dt1 < dt3: " << (dt1 < dt3) << endl;
        cout << "dt1 == dt2: " << (dt1 == dt2) << endl;
        
        // День недели
        cout << "Day of week for 2022-03-15: " << dt1.getDayOfWeek() << endl;
        
        // Пасха
        DateTime easter = DateTime::getEaster(2022);
        cout << "Easter 2022: " << easter << endl;
        
        // Добавление дней
        DateTime dt4 = dt1.addDays(100);
        cout << "100 days after 2022-03-15: " << dt4 << endl;
    } catch (DateTimeException& e) {
        cout << "DateTime error occurred!" << endl;
    }
    
    cout << endl;
}

// Тестирование лунных данных
void testMoonData() {
    cout << "=== Moon Data Tests ===" << endl;
    
    // Тестовые даты
    DateTime dates[] = {
        DateTime(2022, 3, 15),
        DateTime(2022, 6, 21),
        DateTime(2022, 12, 31)
    };
    
    for (const auto& date : dates) {
        cout << "\n--- Testing " << date << " ---" << endl;
        
        MoonResult result = processMoonData(date);
        printResult(date, result);
    }
}

// Интерактивный режим
void interactiveMode() {
    cout << "=== Interactive Moon Data Query ===" << endl;
    
    while (true) {
        cout << "\nEnter date (YYYY-MM-DD) or 'q' to quit: ";
        
        string input;
        getline(cin, input);
        
        if (input == "q" || input == "Q") {
            break;
        }
        
        // Парсим дату из строки
        int year, month, day;
        char sep1, sep2;
        stringstream ss(input);
        ss >> year >> sep1 >> month >> sep2 >> day;
        
        if (ss.fail() || sep1 != '-' || sep2 != '-') {
            cout << "Invalid date format. Use YYYY-MM-DD" << endl;
            continue;
        }
        
        try {
            DateTime target(year, month, day);
            
            clock_t start = clock();
            MoonResult result = processMoonData(target);
            clock_t end = clock();
            
            double time = double(end - start) / CLOCKS_PER_SEC;
            cout << "Search time: " << time << " seconds" << endl;
            
            printResult(target, result);
        } catch (DateTimeException& e) {
            cout << "Invalid date!" << endl;
        }
    }
}

int main() {
    cout << "Moon Data Processor" << endl;
    cout << "===================" << endl;
    
    int choice = 0;
    
    cout << "\nChoose mode:" << endl;
    cout << "1. Run tests" << endl;
    cout << "2. Interactive mode" << endl;
    cout << "3. Quick query" << endl;
    cout << "Enter choice (1-3): ";
    
    cin >> choice;
    cin.ignore(); // Очищаем буфер
    
    switch (choice) {
        case 1:
            testDateTime();
            testMoonData();
            break;
            
        case 2:
            interactiveMode();
            break;
            
        case 3: {
            int year, month, day;
            char sep;
            cout << "Enter date (YYYY-MM-DD): ";
            cin >> year >> sep >> month >> sep >> day;
            
            try {
                DateTime target(year, month, day);
                
                clock_t start = clock();
                MoonResult result = processMoonData(target);
                clock_t end = clock();
                
                double time = double(end - start) / CLOCKS_PER_SEC;
                cout << "Search time: " << time << " seconds" << endl;
                
                printResult(target, result);
            } catch (DateTimeException& e) {
                cout << "Invalid date!" << endl;
            }
            break;
        }
        
        default:
            cout << "Invalid choice!" << endl;
    }
    
    cout << "\nPress Enter to exit...";
    cin.ignore();
    cin.get();
    
    return 0;
}