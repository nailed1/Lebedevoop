#include "DateTime.h"
#include "MoonData.h"
#include <iostream>

using namespace std;

void quickSort(DateTime arr[], int left, int right) {
    if (left >= right) return;
    DateTime pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            DateTime tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++; j--;
        }
    }
    quickSort(arr, left, i - 1);
    quickSort(arr, i, right);
}

void Tests() {
    DateTime today(2026,3,30);
    DateTime yesterday(2026,3,29);
    DateTime today2(2026,3,30);

    cout<<"Print"<<endl;
    cout<<today;

    cout<<"Compare"<<endl;
    cout<<(today==yesterday)<<endl;
    cout<<(today!=yesterday)<<endl;
    cout<<(today>yesterday)<<endl;
    cout<<(today<yesterday)<<endl;
    cout<<(today>=yesterday)<<endl;
    cout<<(today<=today2)<<endl;

    cout<<"Math operation"<<endl;
    double diff = today - yesterday;
    cout<<diff<<endl;

    DateTime future = today + 50;
    cout<<future;

    cout<<"DayOfWeek"<<endl;
    cout <<today.getDayOfWeek()<<endl;

    cout<<"Easter"<<endl;
    DateTime e = DateTime::easter(2026);
    cout << e;
}

void Interactive() {
    cout<<"Input"<<endl;
    DateTime ourDate;
    cin>>ourDate;
    cout<<ourDate;

    cout << "QuickSort" << endl;
    int n;
    cin >> n;
    DateTime* dates = new DateTime[n];

    for (int i = 0; i < n; i++) cin >> dates[i];

    cout << "Before:" << endl;
    for (int i = 0; i < n; i++) cout << dates[i];

    quickSort(dates, 0, n - 1);

    cout << "After:" << endl;
    for (int i = 0; i < n; i++) cout << dates[i];

    delete[] dates;
}

void ExceptionTest() {
    cout<<"Exception"<<endl;
    DateTime wrongDate(2026, 3, 32);
}

int main() {
    // try {
    //     Tests();
    //     Interactive();
    //     ExceptionTest();
    // }
    // catch(const DateTimeException& e) {
    //     cout << "Catch Exception";
    // }

    int y, m, d;
    char sep;
    cin >> y >> sep >> m >> sep >> d;

    DateTime target(y, m, d);

    clock_t start = clock();

    MoonResult result = processMoonData(target);

    if (!result.ok) {
        cout << "File not found" << endl;
        return 1;
    }

    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "Time: " << time << " sec"<<endl;

    printResult(target, result);

    return 0;

    return 0;
}

// 1995-01-01
// 1994-04-26
// 1992-12-05
// 1994-07-12
// 1993-09-11
// 1992-07-21
// 1997-06-29
// 1997-02-06
// 1995-10-31
// 1993-08-18
// 1995-03-02
// 1996-08-12
// 1997-01-28
// 1995-12-15
// 1997-06-09
// 1993-11-10
// 1994-07-05
// 1993-06-06
// 1997-08-21
// 1996-09-28
// 1993-01-14
// 1992-10-02
// 1997-06-13
// 1996-08-18
// 1994-06-08
// 1993-05-04