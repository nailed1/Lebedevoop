#ifndef FIGURES_H
#define FIGURES_H

#include <vector>
#include <string>
#include <memory>
#include <cmath>

// Шаблонный класс Point
template<typename T>
class Point {
private:
    T x, y;
public:
    Point(T x = T(), T y = T()) : x(x), y(y) {}
    
    T getX() const { return x; }
    T getY() const { return y; }
    
    void setX(T newX) { x = newX; }
    void setY(T newY) { y = newY; }
    
    double distanceTo(const Point<T>& other) const {
        double dx = static_cast<double>(x - other.x);
        double dy = static_cast<double>(y - other.y);
        return std::sqrt(dx * dx + dy * dy);
    }
};

// Абстрактный базовый класс
class Figure {
public:
    virtual double calc_area() const = 0;
    virtual double calc_perimeter() const = 0;
    virtual std::string name() const = 0;
    virtual ~Figure() = default;
};

// Круг
template<typename T>
class Circle : public Figure {
private:
    Point<T> center;
    double radius;
public:
    Circle(const Point<T>& c, double r);
    Circle(T x, T y, double r);
    
    double calc_area() const override;
    double calc_perimeter() const override;
    std::string name() const override;
};

// Эллипс
template<typename T>
class Ellipse : public Figure {
private:
    Point<T> center;
    double radiusX, radiusY;
public:
    Ellipse(const Point<T>& c, double rx, double ry);
    Ellipse(T x, T y, double rx, double ry);
    
    double calc_area() const override;
    double calc_perimeter() const override;
    std::string name() const override;
};

// Треугольник (по координатам вершин)
template<typename T>
class Triangle : public Figure {
private:
    Point<T> v1, v2, v3;
public:
    Triangle(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3);
    Triangle(T x1, T y1, T x2, T y2, T x3, T y3);
    Triangle(double a, double b, double c); // по сторонам
    
    double calc_area() const override;
    double calc_perimeter() const override;
    std::string name() const override;
    
private:
    double sideA() const { return v1.distanceTo(v2); }
    double sideB() const { return v2.distanceTo(v3); }
    double sideC() const { return v3.distanceTo(v1); }
    bool isValidTriangle(double a, double b, double c) const;
};

// Прямоугольник
template<typename T>
class Rectangle : public Figure {
private:
    Point<T> v1, v2, v3, v4;
public:
    Rectangle(const Point<T>& p1, const Point<T>& p2, 
              const Point<T>& p3, const Point<T>& p4);
    Rectangle(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4);
    Rectangle(double width, double height); // по сторонам
    
    double calc_area() const override;
    double calc_perimeter() const override;
    std::string name() const override;
    
private:
    double width() const { return v1.distanceTo(v2); }
    double height() const { return v2.distanceTo(v3); }
};

// Многоугольник
template<typename T>
class Polygon : public Figure {
private:
    std::vector<Point<T>> vertices;
public:
    Polygon(const std::vector<Point<T>>& verts);
    
    double calc_area() const override;
    double calc_perimeter() const override;
    std::string name() const override;
};

#endif // FIGURES_H