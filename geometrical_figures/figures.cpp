#include "figures.h"
#include <stdexcept>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Circle
template<typename T>
Circle<T>::Circle(const Point<T>& c, double r) : center(c), radius(r) {
    if (radius <= 0) throw std::invalid_argument("Radius must be positive");
}

template<typename T>
Circle<T>::Circle(T x, T y, double r) : center(x, y), radius(r) {
    if (radius <= 0) throw std::invalid_argument("Radius must be positive");
}

template<typename T>
double Circle<T>::calc_area() const {
    return M_PI * radius * radius;
}

template<typename T>
double Circle<T>::calc_perimeter() const {
    return 2 * M_PI * radius;
}

template<typename T>
std::string Circle<T>::name() const {
    return "Circle";
}

// Ellipse
template<typename T>
Ellipse<T>::Ellipse(const Point<T>& c, double rx, double ry) 
    : center(c), radiusX(rx), radiusY(ry) {
    if (radiusX <= 0 || radiusY <= 0) 
        throw std::invalid_argument("Radius must be positive");
}

template<typename T>
Ellipse<T>::Ellipse(T x, T y, double rx, double ry) 
    : center(x, y), radiusX(rx), radiusY(ry) {
    if (radiusX <= 0 || radiusY <= 0) 
        throw std::invalid_argument("Radius must be positive");
}

template<typename T>
double Ellipse<T>::calc_area() const {
    return M_PI * radiusX * radiusY;
}

template<typename T>
double Ellipse<T>::calc_perimeter() const {
    // Приближенная формула Рамануджана для периметра эллипса
    double a = std::max(radiusX, radiusY);
    double b = std::min(radiusX, radiusY);
    double h = std::pow(a - b, 2) / std::pow(a + b, 2);
    return M_PI * (a + b) * (1 + 3 * h / (10 + std::sqrt(4 - 3 * h)));
}

template<typename T>
std::string Ellipse<T>::name() const {
    return "Ellipse";
}

// ==================== Triangle ====================
template<typename T>
Triangle<T>::Triangle(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3)
    : v1(p1), v2(p2), v3(p3) {
    if (!isValidTriangle(sideA(), sideB(), sideC()))
        throw std::invalid_argument("Invalid triangle vertices");
}

template<typename T>
Triangle<T>::Triangle(T x1, T y1, T x2, T y2, T x3, T y3)
    : v1(x1, y1), v2(x2, y2), v3(x3, y3) {
    if (!isValidTriangle(sideA(), sideB(), sideC()))
        throw std::invalid_argument("Invalid triangle vertices");
}

template<typename T>
Triangle<T>::Triangle(double a, double b, double c) 
    : v1(0, 0), v2(a, 0) {
    if (!isValidTriangle(a, b, c))
        throw std::invalid_argument("Invalid triangle sides");
    
    // Вычисляем координаты третьей вершины
    double cosC = (a*a + b*b - c*c) / (2*a*b);
    double sinC = std::sqrt(1 - cosC*cosC);
    v3 = Point<T>(static_cast<T>(b * cosC), static_cast<T>(b * sinC));
}

template<typename T>
bool Triangle<T>::isValidTriangle(double a, double b, double c) const {
    return (a > 0 && b > 0 && c > 0) && 
           (a + b > c) && (a + c > b) && (b + c > a);
}

template<typename T>
double Triangle<T>::calc_area() const {
    double a = sideA();
    double b = sideB();
    double c = sideC();
    double p = (a + b + c) / 2;
    return std::sqrt(p * (p - a) * (p - b) * (p - c));
}

template<typename T>
double Triangle<T>::calc_perimeter() const {
    return sideA() + sideB() + sideC();
}

template<typename T>
std::string Triangle<T>::name() const {
    return "Triangle";
}

// ==================== Rectangle ====================
template<typename T>
Rectangle<T>::Rectangle(const Point<T>& p1, const Point<T>& p2,
                        const Point<T>& p3, const Point<T>& p4)
    : v1(p1), v2(p2), v3(p3), v4(p4) {
    // Проверяем, что это прямоугольник (диагонали равны)
    double d1 = v1.distanceTo(v3);
    double d2 = v2.distanceTo(v4);
    if (std::abs(d1 - d2) > 1e-6)
        throw std::invalid_argument("Not a rectangle");
}

template<typename T>
Rectangle<T>::Rectangle(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4)
    : v1(x1, y1), v2(x2, y2), v3(x3, y3), v4(x4, y4) {
    double d1 = v1.distanceTo(v3);
    double d2 = v2.distanceTo(v4);
    if (std::abs(d1 - d2) > 1e-6)
        throw std::invalid_argument("Not a rectangle");
}

template<typename T>
Rectangle<T>::Rectangle(double width, double height)
    : v1(0, 0), v2(width, 0), v3(width, height), v4(0, height) {
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("Dimensions must be positive");
}

template<typename T>
double Rectangle<T>::calc_area() const {
    return width() * height();
}

template<typename T>
double Rectangle<T>::calc_perimeter() const {
    return 2 * (width() + height());
}

template<typename T>
std::string Rectangle<T>::name() const {
    return "Rectangle";
}

// ==================== Polygon ====================
template<typename T>
Polygon<T>::Polygon(const std::vector<Point<T>>& verts) : vertices(verts) {
    if (vertices.size() < 3)
        throw std::invalid_argument("Polygon must have at least 3 vertices");
}

template<typename T>
double Polygon<T>::calc_area() const {
    // Формула площади многоугольника через координаты (метод Гаусса)
    double area = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        area += static_cast<double>(vertices[i].getX()) * 
                static_cast<double>(vertices[j].getY());
        area -= static_cast<double>(vertices[j].getX()) * 
                static_cast<double>(vertices[i].getY());
    }
    return std::abs(area) / 2.0;
}

template<typename T>
double Polygon<T>::calc_perimeter() const {
    double perimeter = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        perimeter += vertices[i].distanceTo(vertices[j]);
    }
    return perimeter;
}

template<typename T>
std::string Polygon<T>::name() const {
    std::ostringstream oss;
    oss << "Polygon(" << vertices.size() << ")";
    return oss.str();
}

// Явные инстанциации шаблонов
template class Circle<int>;
template class Circle<float>;
template class Circle<double>;

template class Ellipse<int>;
template class Ellipse<float>;
template class Ellipse<double>;

template class Triangle<int>;
template class Triangle<float>;
template class Triangle<double>;

template class Rectangle<int>;
template class Rectangle<float>;
template class Rectangle<double>;

template class Polygon<int>;
template class Polygon<float>;
template class Polygon<double>;