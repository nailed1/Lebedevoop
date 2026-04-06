#include <iostream>
#include <cassert>
#include <sstream>
#include "DateTime.h"

using namespace std;

void testConstructors() {
    cout << "Testing constructors... ";
    
    DateTime dt1;
    assert(dt1.getYear() == 1970);
    assert(dt1.getMonth() == 1);
    assert(dt1.getDay() == 1);
    assert(dt1.getHour() == 0);
    assert(dt1.getMinute() == 0);
    assert(dt1.getSecond() == 0);
    
    DateTime dt2(2024, 6, 15, 10, 30, 45);
    assert(dt2.getYear() == 2024);
    assert(dt2.getMonth() == 6);
    assert(dt2.getDay() == 15);
    assert(dt2.getHour() == 10);
    assert(dt2.getMinute() == 30);
    assert(dt2.getSecond() == 45);
    
    DateTime dt3(2024, 2, 29, 0, 0, 0);  // Leap year
    assert(dt3.getDay() == 29);
    
    cout << "OK" << endl;
}

void testSetters() {
    cout << "Testing setters... ";
    
    DateTime dt;
    dt.setDate(2023, 5, 10);
    assert(dt.getYear() == 2023);
    assert(dt.getMonth() == 5);
    assert(dt.getDay() == 10);
    
    dt.setTime(14, 30, 45);
    assert(dt.getHour() == 14);
    assert(dt.getMinute() == 30);
    assert(dt.getSecond() == 45);
    
    dt.setDateTime(2022, 12, 25, 8, 15, 30);
    assert(dt.getYear() == 2022);
    assert(dt.getHour() == 8);
    
    dt.setDateTime("2021-07-04T12:00:00");
    assert(dt.getYear() == 2021);
    assert(dt.getMonth() == 7);
    assert(dt.getDay() == 4);
    assert(dt.getHour() == 12);
    
    cout << "OK" << endl;
}

void testNormalization() {
    cout << "Testing normalization... ";
    
    DateTime dt(2024, 1, 1, 0, 0, 60);  // 60 seconds - +1 minute
    assert(dt.getSecond() == 0);
    assert(dt.getMinute() == 1);
    
    dt.setDateTime(2024, 1, 1, 23, 59, 30);
    dt = dt + 60;  // +60 seconds - next day
    assert(dt.getHour() == 0);
    assert(dt.getMinute() == 0);
    assert(dt.getDay() == 2);
    
    dt.setDateTime(2024, 1, 31, 23, 59, 59);
    dt = dt + 1;  // Next day is Feb 1
    assert(dt.getMonth() == 2);
    assert(dt.getDay() == 1);
    
    cout << "OK" << endl;
}

void testPrintFormats() {
    cout << "Testing print formats... ";
    
    DateTime dt(2024, 3, 5, 14, 30, 45);
    
    ostringstream out1;
    dt.printFormat1(out1);
    assert(out1.str() == "05.03.2024");
    
    ostringstream out2;
    dt.printFormat2(out2);
    assert(out2.str() == "5 March 2024");
    
    ostringstream out3;
    dt.printFormat3(out3);
    assert(out3.str() == "24.03.05");
    
    cout << "OK" << endl;
}

void testDifferenceInDays() {
    cout << "Testing difference in days... ";
    
    DateTime dt1(2024, 1, 1);
    DateTime dt2(2024, 1, 11);
    assert(dt2.differenceInDays(dt1) == 10);
    assert(dt1.differenceInDays(dt2) == -10);
    
    DateTime dt3(2023, 1, 1);
    DateTime dt4(2024, 1, 1);
    assert(dt4.differenceInDays(dt3) == 365);
    
    cout << "OK" << endl;
}

void testOperators() {
    cout << "Testing operators... ";

    DateTime dt1(2024, 1, 1, 12, 0, 0);
    DateTime dt2(2024, 1, 1, 12, 0, 0);
    DateTime dt3(2024, 1, 2, 12, 0, 0);

    assert(dt1 == dt2);
    assert(dt1 != dt3);
    assert(dt1 < dt3);
    assert(dt3 > dt1);
    assert(dt1 <= dt2);
    assert(dt1 >= dt2);
    assert(dt1 < dt3);
    assert(dt1 <= dt3);

    DateTime dt4 = dt1 + 3600;  // +1 hour
    assert(dt4.getHour() == 13);

    DateTime dt5 = dt4 - 3600;  // -1 hour
    assert(dt5.getHour() == 12);

    cout << "OK" << endl;
}

void testDateTimeOperators() {
    cout << "Testing DateTime + DateTime and DateTime - DateTime... ";

    DateTime dt1(2024, 1, 1, 10, 30, 0);
    DateTime dt2(2024, 1, 11, 14, 45, 30);

    DateTime diff1 = dt2 - dt1;
    assert(diff1.getDay() == 10);
    assert(diff1.getHour() == 4);
    assert(diff1.getMinute() == 15);
    assert(diff1.getSecond() == 30);

    DateTime dt3(2024, 3, 1, 0, 0, 0);
    DateTime dt4(2024, 2, 1, 0, 0, 0);
    DateTime diff2 = dt3 - dt4;
    assert(diff2.getDay() == 29);
    assert(diff2.getHour() == 0);
    assert(diff2.getMinute() == 0);
    assert(diff2.getSecond() == 0);

    DateTime dt5(2024, 1, 1, 12, 0, 0);
    DateTime duration(0, 0, 0, 2, 30, 0);  // 2 hours 30 minutes
    DateTime dt6 = dt5 + duration;
    assert(dt6.getHour() == 14);
    assert(dt6.getMinute() == 30);
    assert(dt6.getDay() == 1);

    DateTime dt7(2024, 1, 1, 23, 0, 0);
    DateTime duration2(0, 0, 0, 2, 0, 0);  // 2 hours
    DateTime dt8 = dt7 + duration2;
    assert(dt8.getHour() == 1);
    assert(dt8.getDay() == 2);

    cout << "OK" << endl;
}

void testLeapYear() {
    cout << "Testing leap year... ";
    
    DateTime dt1(2024, 2, 29);  // 2024 is leap
    assert(dt1.getMonth() == 2);
    assert(dt1.getDay() == 29);
    
    DateTime dt2(2023, 2, 29);  // is not leap
    assert(dt2.getMonth() == 3);
    assert(dt2.getDay() == 1);
    
    DateTime dt3(2000, 2, 29);  // is leap
    assert(dt3.getMonth() == 2);
    assert(dt3.getDay() == 29);
    
    DateTime dt4(1900, 2, 29);  // is not leap
    assert(dt4.getMonth() == 3);
    assert(dt4.getDay() == 1);
    
    cout << "OK" << endl;
}

void testEaster() {
    cout << "Testing Easter calculation... ";
    
    DateTime dt;
    int easter2024 = dt.calculateEasterDate(2024);
    // Easter 2024 was March 31, which is day 91 (or 90 if counting from 0)
    assert(easter2024 == 91);
    
    int easter1991 = dt.calculateEasterDate(1991);
    assert(easter1991 == 90);
    
    cout << "OK" << endl;
}

void testInput() {
    cout << "Testing input... ";
    
    istringstream in("2025 6 15 10 30 45");
    DateTime dt;
    dt.input(in);
    
    assert(dt.getYear() == 2025);
    assert(dt.getMonth() == 6);
    assert(dt.getDay() == 15);
    assert(dt.getHour() == 10);
    assert(dt.getMinute() == 30);
    assert(dt.getSecond() == 45);
    
    cout << "OK" << endl;
}

int main() {
    cout << "=== DateTime Class Tests ===" << endl << endl;
    
    testConstructors();
    testSetters();
    testNormalization();
    testPrintFormats();
    testDifferenceInDays();
    testOperators();
    testDateTimeOperators();
    testLeapYear();
    testEaster();
    testInput();
    
    cout << endl << "=== All tests passed! ===" << endl;
    
    return 0;
}
