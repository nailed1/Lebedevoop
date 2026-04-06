#include <iostream>
#include <locale>
#include "LineList.h"

using namespace std;

int main(void) {
    setlocale(LC_ALL, "Russian");

    cout << " Тест 1: Базовые операции (вставка) " << endl;
    LineList<int> list;
    
    cout << "Начало: " << list << endl;

    list.insertFirst(10);
    LineListElem<int>* ptr = list.getStart();
    list.insertAfter(ptr, 15);
    
    if (ptr->getNext()) {
        list.insertAfter(ptr->getNext(), 12);
    }
    
    list.insertFirst(7);

    cout << "Шаг 1 (7 10 15 12): " << list << endl;

    cout << "\n Тест 2: Удаление " << endl;
    try {
        list.deleteFirst();
        cout << "После deleteFirst: " << list << endl;
        
        ptr = list.getStart();
        list.deleteAfter(ptr);
        cout << "После deleteAfter(10): " << list << endl;
    } catch (const LineListException& e) {
        cout << "Ошибка: " << e.what() << endl;
    }

    cout << "\n Тест 3: Перебор элементов (Сумма) " << endl;
    int sum = 0;
    for (LineListElem<int>* it = list.getStart(); it; it = it->getNext()) {
        sum += it->getData();
    }
    cout << "Сумма элементов списка: " << sum << endl;

    cout << "\n Тест 4: Работа с другими типами (double) " << endl;
    LineList<double> dList;
    dList.insertFirst(3.14);
    dList.insertFirst(2.5);
    cout << "Список double: " << dList << endl;

    cout << "\n Тест 5: Обработка исключения " << endl;
    LineList<int> emptyList;
    try {
        emptyList.deleteFirst();
    } catch (const LineListException& e) {
        cout << "Поймано исключение при удалении из пустого списка: " << e.what() << endl;
    }

    return 0;
}