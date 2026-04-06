#include "array.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

int josephusArray(int N, int k) {
    if (N <= 0 || k <= 0) return 0;

    Array list(N);
    for (int i = 1; i <= N; i++)
        list.insert(i);

    int current = 0;

    while (list.getSize() > 1) {
        current = (current + k - 1) % list.getSize();
        list.remove(current);
        if (current == list.getSize())
            current = 0;
    }

    return list[0];
}

int main() {
    int N, k;
    cout << "Введите N и k: ";
    cin >> N >> k;

    clock_t start = clock();
    int result = josephusArray(N, k);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "Ответ: " << result << endl;
    cout << "Время: " << elapsed << " сек" << endl;

    int testNs[] = {1000, 5000, 10000, 50000, 100000, 1000000};
    int testCount = 7;

    ofstream csv("josephus_results.csv");
    csv << "N,Answer,Time_sec" << endl;

    cout << "\nКонтрольная проверка (k=2):" << endl;
    cout << "N\t\tОтвет\t\tВремя (сек)" << endl;

    for (int i = 0; i < testCount; i++) {
        clock_t t1 = clock();
        int ans = josephusArray(testNs[i], 2);
        clock_t t2 = clock();
        double t = (double)(t2 - t1) / CLOCKS_PER_SEC;

        cout << testNs[i] << "\t\t" << ans << "\t\t" << t << endl;
        csv << testNs[i] << "," << ans << "," << t << endl;
    }

    csv.close();
    cout << "\nРезультаты сохранены в josephus_results.csv" << endl;

    return 0;
}
