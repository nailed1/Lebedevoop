#include <iostream>
#include <locale>
#include "function.h"
#include "hyperbola.h"
#include "parabola.h"
#include "exponenta.h"
#include "polinom.h"

void demonstrateFunction(Function* func, double x, double intervalStart, double intervalEnd) {
    func->print();
    
    // Вычисление значения в точке
    std::cout << "\n1. Вычисление значения функции:" << std::endl;
    std::cout << "   f(" << x << ") = " << func->compute(x) << std::endl;
    
    // Поиск минимума
    std::cout << "\n2. Поиск минимума на интервале [" << intervalStart << ", " << intervalEnd << "]:" << std::endl;
    func->findMin(intervalStart, intervalEnd);
    
    // Поиск максимума
    std::cout << "\n3. Поиск максимума на интервале [" << intervalStart << ", " << intervalEnd << "]:" << std::endl;
    func->findMax(intervalStart, intervalEnd);
    
    // Интегрирование
    std::cout << "\n4. Вычисление определённого интеграла:" << std::endl;
    func->integrate(intervalStart, intervalEnd);
    
    // Дифференцирование
    std::cout << "\n5. Вычисление производной:" << std::endl;
    func->differentiate(x);
    
    std::cout << "\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    std::cout << "   АБСТРАКТНЫЙ КЛАСС FUNCTION" << std::endl;
    std::cout << "   Демонстрация возможностей" << std::endl;
    
    // Тестовые параметры
    double testX = 2.0;
    double intervalStart = -5.0;
    double intervalEnd = 5.0;
    
    // 1. Гипербола: y = 1/(x+1) + 2
    std::cout << "\n>>> ТЕСТИРОВАНИЕ ГИПЕРБОЛЫ" << std::endl;
    Hyperbola hyperbola(1.0, 1.0, 2.0);
    demonstrateFunction(&hyperbola, testX, -0.9, 5.0);  // Избегаем разрыва в x = -1
    
    // 2. Парабола: y = x² - 4x + 3
    std::cout << "\n>>> ТЕСТИРОВАНИЕ ПАРАБОЛЫ" << std::endl;
    Parabola parabola(1.0, -4.0, 3.0);
    demonstrateFunction(&parabola, testX, -2.0, 6.0);
    
    // 3. Экспонента: y = 2 * e^(0.5x) - 1
    std::cout << "\n>>> ТЕСТИРОВАНИЕ ЭКСПОНЕНТЫ" << std::endl;
    Exponenta exponenta(2.0, 0.5, -1.0);
    demonstrateFunction(&exponenta, testX, -3.0, 3.0);
    
    // 4. Полином 3-й степени: y = x³ - 2x² + x - 1
    std::cout << "\n>>> ТЕСТИРОВАНИЕ ПОЛИНОМА" << std::endl;
    std::vector<double> coeffs = {-1.0, 1.0, -2.0, 1.0};  // -1 + x - 2x² + x³
    Polinom polinom(coeffs);
    demonstrateFunction(&polinom, testX, -2.0, 3.0);
    
    // 5. Дополнительный пример: полином 4-й степени
    std::cout << "\n>>> ДОПОЛНИТЕЛЬНЫЙ ПРИМЕР - ПОЛИНОМ 4-Й СТЕПЕНИ" << std::endl;
    Polinom polinom4(4);
    polinom4.setCoefficient(0, 5.0);   // 5
    polinom4.setCoefficient(1, 0.0);   // 0x
    polinom4.setCoefficient(2, -3.0);  // -3x²
    polinom4.setCoefficient(3, 0.0);   // 0x³
    polinom4.setCoefficient(4, 1.0);   // x⁴
    demonstrateFunction(&polinom4, 1.5, -3.0, 3.0);
    
    std::cout << "   ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ УСПЕШНО!" << std::endl;
    
    return 0;
}