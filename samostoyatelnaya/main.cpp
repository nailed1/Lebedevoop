#include <iostream>
#include "TextFile.h"

int main() {
    // --- 1. Загрузка одного файла ---
    TextFile<FormatRowFile> f1("STARLINK/chirp_20210126_084300_003_out.dat");
    std::cout << "f1 rows: " << f1.size() << "\n";

    // --- 2. Загрузка второго файла ---
    TextFile<FormatRowFile> f2("STARLINK/chirp_20210126_084400_003_out.dat");
    std::cout << "f2 rows: " << f2.size() << "\n";

    // --- 3. Конструктор по маске ---
    TextFile<FormatRowFile> all("STARLINK", "chirp_20210126_0843*");
    std::cout << "mask rows: " << all.size() << "\n";

    // --- 4. appendFrom: дописать f2 в конец f1 ---
    TextFile<FormatRowFile> appended = f1;
    appended.appendFrom(f2);
    std::cout << "after appendFrom: " << appended.size() << " rows\n";

    // --- 5. uniqueLines / commonLines ---
    TextFile<FormatRowFile> unique  = f1.uniqueLines(f2);
    TextFile<FormatRowFile> common  = f1.commonLines(f2);
    std::cout << "unique rows: " << unique.size() << "\n";
    std::cout << "common rows: " << common.size() << "\n";

    // --- 6. Сортировка по колонке 6 (R(km)), убывание ---
    TextFile<FormatRowFile> sorted = f1;
    sorted.sortByField(6, false); // колонка R(km)
    std::cout << "top-3 by R(km) desc:\n";
    for (int i = 0; i < 3 && i < (int)sorted.size(); ++i)
        std::cout << "  " << sorted.data[i].toString() << "\n";

    // --- 7. Сумма по колонке 4 (Amp) ---
    double ampSum = f1.sumField(4);
    std::cout << "sum Amp (col 4): " << ampSum << "\n";

    // --- 8. Операторы ---
    // += : добавить в f1 неповторяющиеся строки из f2
    TextFile<FormatRowFile> op1 = f1;
    op1 += f2;
    std::cout << "op1 (f1 += f2): " << op1.size() << " rows\n";

    // + : объединение без дублей
    TextFile<FormatRowFile> op2 = f1 + f2;
    std::cout << "op2 (f1 + f2):  " << op2.size() << " rows\n";

    // -= : удалить из f1 строки, есть в f2
    TextFile<FormatRowFile> op3 = f1;
    op3 -= f2;
    std::cout << "op3 (f1 -= f2): " << op3.size() << " rows\n";

    // - : симметричная разность
    TextFile<FormatRowFile> op4 = f1 - f2;
    std::cout << "op4 (f1 - f2):  " << op4.size() << " rows\n";

    // --- 9. Сохранение результата ---
    op2.save("result_union.dat");
    std::cout << "saved result_union.dat\n";

    return 0;
}
