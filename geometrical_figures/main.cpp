#include "figures.h"
#include <iostream>
#include <iomanip>

int main() {
    std::vector<Figure*> figures;
    
    // Добавляем разные фигуры
    figures.push_back(new Circle<double>(0.0, 0.0, 5.0));
    figures.push_back(new Ellipse<double>(1.0, 1.0, 4.0, 2.0));
    figures.push_back(new Triangle<double>(3.0, 4.0, 5.0));
    
    // Прямоугольник по координатам
    figures.push_back(new Rectangle<double>(
        Point<double>(0, 0), Point<double>(4, 0),
        Point<double>(4, 3), Point<double>(0, 3)
    ));
    
    // Многоугольник (пятиугольник)
    std::vector<Point<double>> pentagon = {
        Point<double>(0, 0), Point<double>(2, 0), Point<double>(3, 2),
        Point<double>(1, 4), Point<double>(-1, 2)
    };
    figures.push_back(new Polygon<double>(pentagon));
    
    // Ещё фигуры
    figures.push_back(new Circle<int>(10, 20, 7));
    figures.push_back(new Rectangle<int>(5.0, 3.0));
    
    // Вывод информации
    std::cout << std::fixed << std::setprecision(3);
    
    double totalArea = 0.0;
    double totalPerimeter = 0.0;
    
    for (size_t i = 0; i < figures.size(); ++i) {
        Figure* fig = figures[i];
        double area = fig->calc_area();
        double perim = fig->calc_perimeter();
        
        totalArea += area;
        totalPerimeter += perim;
    }
    
    std::cout << "Total figures: " << figures.size() << "\n";
    std::cout << "Sum of all areas: " << totalArea << "\n";
    std::cout << "Sum of all perimeters: " << totalPerimeter << "\n";
    
    // Очистка памяти
    for (Figure* fig : figures) {
        delete fig;
    }
    
    return 0;
}